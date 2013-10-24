#include <stdlib.h>
#include <stdio.h>

#define N   1 
#define S   2
#define E   4
#define W   8
#define width     16
#define height    8
#define weight    2

typedef unsigned char u8;
typedef unsigned int u32;

static u8 grid[height][width];

#define _rand(MAX) (rand()%(MAX))
#define print(str) printf("%s",(str))

void prt_maze()
{
	u32 x, y;
	print(" ");
	for(x = 0; x < width*2-1; x++)
		print("_");
	print("\n");
	for(y = 0; y < height; y++)
	{
		print("|");
		for(x = 0; x < width; x++)
		{
			u8 cell = grid[y][x];
			// last row or not
			if(cell == 0 && (y+1) < height && grid[y+1][x] == 0)
				print(" ");
			else
				print((cell & S) ? " " : "_");
			// last col or not
			if(cell == 0 && (x+1) < width && grid[y][x+1] == 0)
				print(((y+1) < height && (grid[y+1][x] == 0 || grid[y+1][x+1] == 0)) ? " " : "_");
			else if(cell & E)
				print(((cell | grid[y][x+1]) & S) ? " " : "_");
			else
				print ("|");
		}
		print("\n");
	}
}
///////////////////////////////////////////////
//  Sidewinder algorithm
///////////////////////////////////////////////
void gen_maze()
{
	u32 x, y, cell;
	for(y = 0; y < height; y++)
	{
		u32 run_start = 0;
		for(x = 0; x < width; x++)
		{
			// not first row, is last col or not
			if(y > 0 && (x+1 == width || _rand(weight) == 0))
			{
				cell = run_start + _rand(x - run_start + 1);
				grid[y][cell] |= N;
				grid[y-1][cell] |= S;
				run_start = x+1;
			}
			// not last col
			else if (x+1 < width)
			{
				grid[y][x] |= E;
				grid[y][x+1] |= W;
			}
		}
	}
}

#include <time.h>
int main()
{
    srand(time(NULL));
	gen_maze();
	prt_maze();
	return 0;
}

