#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <arpa/inet.h> /*inet_addr, inet_ntoa, ntohs etc*/
#include <netinet/in.h>
#include <netdb.h>

//#define DNS_SERVER  "114.114.114.114"
//#define DNS_SERVER  "42.120.21.30"
#define DNS_SERVER  "8.8.8.8"
struct DNS_HEADER
{
    unsigned short id;      //0 identification number

    unsigned char rd :1;    //2 recursion desired
    unsigned char tc :1;    //2 truncated message
    unsigned char aa :1;    //2 authoritive answer
    unsigned char opcode :4;//2 purpose of message
    unsigned char qr :1;    //2 query/response flag

    unsigned char rcode :4; //3 response code
    unsigned char cd :1;    //3 checking disabled
    unsigned char ad :1;    //3 authenticated data
    unsigned char z :1;     //3 its z! reserved
    unsigned char ra :1;    //3 recursion available

    unsigned short q_count;     //4 number of question entries
    unsigned short ans_count;   //6 number of answer entries
    unsigned short auth_count;  //8 number of authority entries
    unsigned short add_count;   //10 number of resource entries
};

static int is_little_endian()
{
    union w
    {
        int a;
        char b;
    } c;
    c.a = 1;
    return(c.b == 1);
}

/* decode from big-endian to short */
static unsigned short decode2short (const unsigned char * buffer)
{
    unsigned short v = 0;
    unsigned char *p = (unsigned char *) &v;
    if(is_little_endian()) {
        p[0] = buffer[1];
        p[1] = buffer[0];
    } else {
        p[0] = buffer[0];
        p[1] = buffer[1];
    }
    return v;
}

void decode_dns_response (unsigned char * buffer, const char * hostna, char * ip)
{
    int i, j;
    int h_len = strlen(hostna);
    unsigned char * p = buffer + 6; /* get ans_count */
    unsigned short ans_count = decode2short(p);
    /* header(12) + (host length+1) + eof sign(1) + qtype(2) + qclass(2) */
    /* skip query answer field */
    p = buffer + 12 + (h_len + 1) + 1 + 4;

    for(i = 0; i < ans_count; i++)
    {
        char flag = p[0];
        if((flag & 0xc0) == 0xc0) {
            p += 2;
        }
        else {
            p += 1 + h_len + 1;
        }
        short query_type = decode2short(p);

        p += 8;
        int data_len = decode2short(p);
        p += 2; //move to data area

        if (query_type == 0x0001)
        {
            bzero(ip, NI_MAXHOST);
            for(j = 0; j < data_len; j ++)
            {
                int v = p[0];
                v = v>0 ? v : 0x0ff&v;
                char tmp[4];
                sprintf(tmp, "%d", v);
                strcat(ip, tmp);
                if(j < data_len - 1) {
                    strcat(ip, ".");
                }
                p++;
            }
            printf("*%s\n", ip);
        }
        else
        {
            p += data_len;
        }
    }
}

char * build_request_data (const char * hostname,int * ret_size)
{
    /* header(12) + (host length+1) + eof sign(1) + qtype(2) + qclass(2) */
    int size = 12 + (strlen(hostname)+1) + 1 + 4;
    char * buffer = (char *) malloc (size);
    char * pbuf = buffer;
    char * hostname1 = (char *)malloc(strlen(hostname));
    strcpy(hostname1, hostname);

    bzero(pbuf, size);

    unsigned short seq = rand();
    memcpy(pbuf, &seq, 2);
    pbuf += 2;
    unsigned char header[10] = {0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00};
    memcpy(pbuf, header, 10);
    pbuf += 10;

    char * pstr = strtok(hostname1, ".");
    while(pstr != NULL)
    {
        /* write name in 3www6google3com format */
        unsigned char len = strlen(pstr);
        memcpy(pbuf, &len, 1);
        pbuf += 1;

        memcpy(pbuf, pstr, len);
        pbuf += len;

        pstr = strtok(NULL, ".");
    }
    /* EOF */
    pbuf += 1;

    unsigned char extra_data[4] = {
        0x00, 0x01, /* qtype */
        0x00, 0x01  /* qclass */
    };
    memcpy(pbuf, extra_data, 4);

    *ret_size = size;
    return buffer;
}

void dns_resolve(const char * hostname, char * out_ip)
{
    int s, len;
    char * request_buffer;
    struct sockaddr_in dest;
    unsigned char recv_buf[1024];

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = inet_addr(DNS_SERVER);
    socklen_t addr_len = sizeof(struct sockaddr_in);

    request_buffer = build_request_data(hostname, &len);
    if(sendto(s, request_buffer, len, 0, (struct sockaddr*)&dest, addr_len) < 0)
        perror("sendto failed");

    len = recvfrom(s, recv_buf, sizeof(recv_buf),0, (struct sockaddr*)&dest,&addr_len);
    decode_dns_response(recv_buf, hostname, out_ip);

    free(request_buffer);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: ./a.exe hostname\n");
        return 0;
    }
    else
    {
        char ip[NI_MAXHOST];
        dns_resolve(argv[1], ip);
    }
    return 0;
}
