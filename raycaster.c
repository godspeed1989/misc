/**************************************
 * A simple raycaster, with the
 * view to turn into HDL
 **************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#define VIEWS 32
#define HREZ 1024
#define VREZ 768

float pos_x = 5.2, pos_y = 4.8;
float sines[VIEWS], scales[HREZ/2+1], data[HREZ];
unsigned char tex[HREZ];
unsigned char colour[HREZ];

unsigned char maze[8][8] = {
	{1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0},
	{1,0,0,0,0,0,0,0},
	{1,1,0,0,0,0,0,0},
	{1,0,0,0,0,0,0,0},
	{1,0,0,0,0,0,1,1},
	{1,0,0,0,0,0,0,0},
	{1,0,1,0,0,0,0,0}
};

unsigned char texture[8][8] = {
	{0,0,0,1,0,0,0,0},
	{0,0,0,1,0,0,0,0},
	{0,0,0,1,0,0,0,0},
	{1,1,1,1,1,1,1,1},
	{0,0,0,0,0,0,0,1},
	{0,0,0,0,0,0,0,1},
	{0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1}
};

/*************************************************/
static void setupTables(void) /* Precalculate these for BRAM */
{
	int i;
	for(i = 0; i < VIEWS; i++)
		sines[i] = sin(i*2*3.141592/VIEWS);
	for(i = 0; i < HREZ/2+1; i++)
		scales[i] = 1/sqrt(1 + (float)i/1024 * (float)i/1024);
}

/*************************************************/
static float cast(float vx, float vy, unsigned char *tex)
{
	int i;
	int maze_x = (int)(pos_x)&7, maze_y = (int)(pos_y)&7;
	/* vx & vy are a unit vector for the ray to be cast from pos_x,pos_y */
	float ax,ay, tex_x, tex_y;
	float deltad_deltax, deltad_deltay;
	float deltax_deltay, deltay_deltax;
	float distance_x, distance_y;
	int dir_x, dir_y;

	assert(vx*vx+vy*vy > 0.9999 && vx*vx+vy*vy < 1.0001);

	dir_x = (vx < 0 ? -1 : 1);
	dir_y = (vy < 0 ? -1 : 1);

	/* ax, ay are the absolute values of vx & vy */
	ax = fabs(vx);
	ay = fabs(vy);

	deltad_deltax = (ax == 0 ? 4095 : 1/ax);
	deltad_deltay = (ay == 0 ? 4095 : 1/ay);
	deltay_deltax = (ax == 0 ? 0 : vy/ax);
	deltax_deltay = (ay == 0 ? 0 : vx/ay);

	/* Work out how far the ray is from hitting the cell boundary in
	   both the x and y axis */
	if(dir_x > 0) distance_x = floor(pos_x+1)-pos_x;
	else if(dir_x < 0) distance_x = pos_x - floor(pos_x);

	if(dir_y > 0) distance_y = floor(pos_y+1)-pos_y;
	else if(dir_y < 0) distance_y = pos_y - floor(pos_y);

	/* Use that to set up the the textures */
	tex_x = pos_x - floor(pos_x) + distance_x * deltay_deltax;
	tex_y = pos_y - floor(pos_y) + distance_y * deltax_deltay;

	/* Switch those distances over form distance on an axis to the distance
	   along the sight vector */
	distance_x *= deltad_deltax;
	distance_y *= deltad_deltay;

	for(i = 0; i < 32; i++)
	{
		/* Will we hit the bondary in the X axis first? */
		if(distance_x < distance_y)
		{
			maze_x = (maze_x + dir_x) & 7;
			if(maze[maze_x][maze_y]) {
				*tex = (int)(tex_x*64)&7;
				return distance_x;
			}
			tex_x += deltay_deltax;
			distance_x += deltad_deltax;
		} 
		else
		{
			maze_y = (maze_y + dir_y) & 7;
			if(maze[maze_x][maze_y]) {
				*tex = (int)(tex_y*64)&7;
				return distance_y;
			}
			tex_y += deltax_deltay;
			distance_y += deltad_deltay;
		}
	}
	return 0;
}
/*************************************************/
void buildFrameData(int v)
{
	int i;

	for(i = 0; i < HREZ; i++)
	{
		 int v1 = v;
		 int v2 = (v+VIEWS/4)&(VIEWS-1);
		 float s, vx, vy, d;
		 s = scales[i < HREZ/2 ? HREZ/2-i : i-HREZ/2 ];
		 vx = (sines[v2] + (i-HREZ/2)*sines[v1]/1024) * s;
		 vy = (sines[v1] - (i-HREZ/2)*sines[v2]/1024) * s;
		 d = cast(vx, vy, tex+i);
		 data[i] = (d * s);
	}
}
/*************************************************/
void outputFrame(int i) /* Will actually be VGA */
{
	int x,y;
	char fname[10];
	FILE *f;
	sprintf(fname, "out%02i.ppm", i);
	f = fopen(fname, "wb");
	if(f == NULL) exit(2);
	fprintf(f,"P6\n%i %i\n255\n", HREZ, VREZ);

	for(y = 0; y < HREZ; y++)
	{
		for(x = 0; x < HREZ; x++)
		{
			int wall = (y-VREZ/2)*data[x]+384;
			if(wall < 0)//ceiling
			{
				putc(128,f); putc(128,f); putc(128,f);
			}
			else if(wall > 1023)//floor
			{
				putc(64,f); putc(64,f); putc(64,f);
			}
			else if(texture[(wall>>4)&0x7][tex[x]] != 0)
			{
				putc(192,f); putc(192,f); putc(192,f);
			}
			else
			{
				putc(192,f); putc(64,f); putc(64,f);
			}
		}
	}
	fclose(f);
}
/*************************************************/
int main(int argc, char *argv[])
{
	int i;
	setupTables();
	for(i = 0; i < VIEWS; i++)
	{
		buildFrameData(i);
		outputFrame(i);
	}
	return 0;
}

