#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    time_t t;
    struct tm gm, loc;
    struct tm *gmp, *locp;

    t = time(NULL);
    printf("Seconds since the Epoch (1 Jan 1970): %ld\n\n", (long) t);

    /* gmtime() */
    gmp = gmtime(&t);
    if (gmp == NULL)
        perror("gmtime");
    gm = *gmp;        /* Save local copy */
    printf("Broken down by gmtime():\n");
    printf("  year=%d mon=%d mday=%d hour=%d min=%d sec=%d ", gm.tm_year,
            gm.tm_mon, gm.tm_mday, gm.tm_hour, gm.tm_min, gm.tm_sec);
    printf("wday=%d yday=%d isdst=%d\n", gm.tm_wday, gm.tm_yday, gm.tm_isdst);

    /* localtime */
    locp = localtime(&t);
    if (locp == NULL)
        perror("localtime");
    loc = *locp;        /* Save local copy */
    printf("Broken down by localtime():\n");
    printf("  year=%d mon=%d mday=%d hour=%d min=%d sec=%d ",
            loc.tm_year, loc.tm_mon, loc.tm_mday,
            loc.tm_hour, loc.tm_min, loc.tm_sec);
    printf("wday=%d yday=%d isdst=%d\n\n",
            loc.tm_wday, loc.tm_yday, loc.tm_isdst);

    printf("asctime() formats the gmtime() value as: %s", asctime(&gm));
    printf("ctime() formats the time() value as:     %s", ctime(&t));

    printf("mktime() of gmtime() value:    %ld secs\n", (long) mktime(&gm));
    printf("mktime() of localtime() value: %ld secs\n", (long) mktime(&loc));

    exit(EXIT_SUCCESS);
}
