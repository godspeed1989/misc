#include <cstdio>
#include <cassert>
#include <vector>
#include <iostream>
using namespace std;

typedef char GENE_T;
#define _A 'A'
#define _T 'T'
#define _C 'C'
#define _G 'G'
#define _N '-'

typedef unsigned long score_t;
typedef struct table_t
{
	score_t score;
	char mark;
	table_t(score_t s=-1, const char m='?') : score(s), mark(m) {}
}table_t;

#define MAX_LEN 11000
static table_t tlb[MAX_LEN][MAX_LEN];
static vector<GENE_T> seq1;
static vector<GENE_T> seq2;

#define is_valid(c) ((c==_A)||(c==_T)||(c==_C)||(c==_G))
void load_data(const char *file, vector<GENE_T>& s1, vector<GENE_T>& s2)
{
	char c;
	FILE *fin;
	fin = fopen(file, "r");
	assert(file);
	while(!feof(fin) && (c=fgetc(fin))!='\n')
		if(is_valid(c)) s1.push_back(c);
	while(!feof(fin) && (c=fgetc(fin))!='\n')
		if(is_valid(c)) s2.push_back(c);
	fclose(fin);
}

void output_tlb(int t = 0)
{
	printf("Table:\n%c", '\\');
	for(size_t i = 1; i <= seq2.size(); ++i)
		printf(" %4c", seq2[i-1]);
	printf("\n");
	for(size_t i = 1; i <= seq1.size(); ++i)
	{
		printf("%c", seq1[i-1]);
		for(size_t j = 1; j <= seq2.size(); ++j)
		{
			if(!t)
				printf(" %4ld", tlb[i][j].score);
			else
				printf(" %4c", tlb[i][j].mark);
		}
		printf("\n");
	}
}

void output_result(void)
{
	size_t i = 1, j = 1;
	score_t s = 0;
	vector<GENE_T> __seq1, __seq2;
	vector<score_t> __score;
	while(i<=seq1.size() && j<=seq2.size())
	{
		switch(tlb[i][j].mark)
		{
			case '\\':
				__seq1.push_back(seq1[i-1]);
				__seq2.push_back(seq2[j-1]);
				s = (seq1[i-1] == seq2[j-1]) ? 0 : 1;
				i++; j++;
				break;
			case '-':
				__seq1.push_back(_N);
				__seq2.push_back(seq2[j-1]);
				s = 2;
				j++;
				break;
			case '|':
				__seq1.push_back(seq1[i-1]);
				__seq2.push_back(_N);
				s = 2;
				i++;
				break;
			case '.':
				s = tlb[1][1].score;
				i++; j++;
				break;
			default:
				printf("error: unknown mark %c\n", tlb[i][j].mark);
				assert(0);
				break;
		}
		__score.push_back(s);
	}
	for(i = 0; i < __seq1.size(); ++i)
		cout << __seq1[i] << "  ";
	printf("\n");
	for(i = 0; i < __seq2.size(); ++i)
		cout << __seq2[i] << "  ";
	printf("\n");
	for(i = 0; i < __score.size(); ++i)
		cout << __score[i] << "  ";
	printf("\n");
}

void calculate(const char *file)
{
	/* load data from file */
	load_data(file, seq2, seq1);
	cout<< "seq1: " << seq1.size() << endl
		<< "seq2: " << seq2.size() << endl;
	/* padding the NULL at the end of seq */
	seq1.push_back(_N);
	seq2.push_back(_N);
	/* initial table variable */
	for(size_t i = 1; i <= seq1.size(); ++i)
		tlb[i][seq2.size()+1] = table_t(2*MAX_LEN, '?');
	for(size_t j = 1; j <= seq2.size(); ++j)
		tlb[seq1.size()+1][j] = table_t(2*MAX_LEN, '?');
	tlb[seq1.size()][seq2.size()] = table_t(0, '.');
	/* construct table */
	for(size_t i = seq1.size(); i != 0; --i)
	{
		for(size_t j = seq2.size(); j != 0; --j)
		{
			if(i == seq1.size() && j == seq2.size())
				continue;
			if(seq1[i-1] == seq2[j-1])
			{
				tlb[i][j] = table_t(tlb[i+1][j+1].score+0, '\\');
			}
			else
			{
				score_t R, D, DR;
				R = tlb[i][j+1].score + 2;
				D = tlb[i+1][j].score + 2;
				DR = tlb[i+1][j+1].score + 1;
				// find min of {R, D, DR}
				if(R < D)
				{
					if(R < DR)
						tlb[i][j] = table_t(R, '-');
					else
						tlb[i][j] = table_t(DR, '\\');
				}
				else
				{
					if(D < DR)
						tlb[i][j] = table_t(D, '|');
					else
						tlb[i][j] = table_t(DR, '\\');
				}
			}
		}
	}
	/* result */
	cout << "score is " << tlb[1][1].score << endl;
}

int main(int c, const char* v[])
{
	if(c < 2) return 1;
	calculate(v[1]);
	output_result();
	output_tlb();
	output_tlb(1);
	return 0;
}


