/*********************************************
* mandelbrot.c : Calculate the Mandelbrot set.
*
* Compile with: gcc -o mandelbrot mandelbrot.c
* Usage: ./mandelbrot x y zoom
*
* Try    ./mandelbrot 0.418 0.199 100
*********************************************/
#include <stdio.h>

/* Set the image size here */
#define HRES 1024
#define VRES 768

unsigned char mandelbrot(double cx, double cy)
{
	int i = 0;
	double x = cx, y = cy;
	while(i < 255 && x*x+y*y < 4.0)
	{
		double t = x;
		x = x*x-y*y+cx;
		y = 2*t*y+cy;
		i++;
	}
	return i;
}

int main(int argc, char *argv[])
{
	int x, y;
	FILE *f;
	double center_x = 0.0,center_y = 0.0;
	double zoom = 1.0, scale;

	f = fopen("out.pbm","w");
	if(f == NULL) {
		printf("Can't open output file\n");
		return 0;
	}

	if(argc == 4) {
		sscanf(argv[1], "%lf", &center_x);
		sscanf(argv[2], "%lf",&center_y);
		sscanf(argv[3], "%lf", &zoom);
	}

	printf("Center (%f,%f), zoom %f\n",
		center_x, center_y, zoom);

	/* Adjust scale for image size */
	scale = 4.0 / zoom / HRES;

	/* Output the image header */
	fprintf(f, "P3\n%i %i\n255\n", HRES, VRES);

	for(y = -VRES/2; y < -VRES/2+VRES; y++)
	{
		for(x = -HRES/2; x < -HRES/2+HRES; x++)
		{
			unsigned char i;
			float pixel_x = center_x + scale * x;
			float pixel_y = center_y + scale * y;

			i = mandelbrot(pixel_x, pixel_y);

			/* 'Magic' to convert i to a colour */
			i = i + 1;
			fprintf(f, "%d ", i); /* Red */
			fprintf(f, "%d ", i*8); /* Green */
			fprintf(f, "%d ", i*64); /* Blue */
		}
        fprintf(f, "\n");
	}
	fclose(f);
	return 0;
}
