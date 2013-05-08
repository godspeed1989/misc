
#include <linux/fb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdio.h>

const char * FB_DEV = "/dev/fb0";


// 画大小为width*height的同色矩阵，8alpha+8reds+8greens+8blues
void drawRect_rgb32(int x0, int y0, int width, int height, __u32 color32,
                    struct fb_var_screeninfo* var, struct fb_fix_screeninfo* fix,
                    void* frameBuffer)
{
	const int bytesPerPixel = 4;
	const int stride = fix->line_length / bytesPerPixel;

	__u32 *dest = (__u32*) (frameBuffer)
		+ (y0 + var->yoffset) * stride + (x0 + var->xoffset);

	int x, y;
	for (y = 0; y < height; ++y)
	{
		for (x = 0; x < width; ++x)
		{
		   dest[x] = color32;
		}
		dest += stride;
	}
}
// 画大小为width*height的同色矩阵，5reds+6greens+5blues
void drawRect_rgb16(int x0, int y0, int width, int height, __u32 color32,
                    struct fb_var_screeninfo* var, struct fb_fix_screeninfo* fix,
                    void* frameBuffer)
{
	const int bytesPerPixel = 2;
	const int stride = fix->line_length / bytesPerPixel;
	const int red = (color32 & 0xff0000) >> (16 + 3); // 8-5=3
	const int green = (color32 & 0xff00) >> (8 + 2); // 8-6=2
	const int blue = (color32 & 0xff) >> 3;
	const __u16 color16 = blue | (green << 5) | (red << (5 + 6));

	__u16 *dest = (__u16*) (frameBuffer)
		+ (y0 + var->yoffset) * stride + (x0 + var->xoffset);

	int x, y;
	for (y = 0; y < height; ++y)
	{
		for (x = 0; x < width; ++x)
		{
			dest[x] = color16;
		}
		dest += stride;
	}
}
//画大小为width*height的同色矩阵，5reds+5greens+5blues
void drawRect_rgb15(int x0, int y0, int width, int height, __u32 color32,
                    struct fb_var_screeninfo* var, struct fb_fix_screeninfo* fix,
                    void* frameBuffer)
{
	const int bytesPerPixel = 2;
	const int stride = fix->line_length / bytesPerPixel;
	const int red = (color32 & 0xff0000) >> (16 + 3); // 8-5=3
	const int green = (color32 & 0xff00) >> (8 + 3);
	const int blue = (color32 & 0xff) >> 3;
	const __u16 color15 = blue | (green << 5) | (red << (5 + 5)) | 0x8000;

	__u16 *dest = (__u16*) (frameBuffer)
		+ (y0 + var->yoffset) * stride + (x0 + var->xoffset);

	int x, y;
	for (y = 0; y < height; ++y)
	{
		for (x = 0; x < width; ++x)
		{
		   dest[x] = color15;
		}
		dest += stride;
	}
}
void drawRect(int x0, int y0, int width, int height, int color,
              struct fb_var_screeninfo* var, struct fb_fix_screeninfo* fix,
              void* frameBuffer)
{
	switch (var->bits_per_pixel)
	{
	case 32:
		drawRect_rgb32(x0, y0, width, height, color, var, fix, frameBuffer);
		break;
	case 16:
		drawRect_rgb16(x0, y0, width, height, color, var, fix, frameBuffer);
		break;
	case 15:
		drawRect_rgb15(x0, y0, width, height, color, var, fix, frameBuffer);
		break;
	default:
		printf("Warning: drawRect() not support for color depth %d\n",
				var->bits_per_pixel);
		break;
	}
}
/*
 * print out frame buffer information and draw rectangles
 */
void print_fb_info()
{
	int fh;
	struct fb_var_screeninfo var;
	struct fb_fix_screeninfo fix;

	if((fh = open(FB_DEV, O_RDWR)) == -1)
	{
		printf("fb dev %s open err\n", FB_DEV);
		return;
	}
	if(ioctl(fh, FBIOGET_VSCREENINFO, &var))
	{
		printf("get fb var info err\n");
		return;
	}
	printf("resolution: x=%d y=%d\n", var.xres, var.yres);
	printf("virtual resolution: %d %d\n", var.xres_virtual, var.yres_virtual);
	printf("bits per pixel: %d\n", var.bits_per_pixel);
	printf("red: offset=%d length=%d msb_right=%d\n", var.red.offset, var.red.length, var.red.msb_right);
	printf("green: offset=%d length=%d msb_right=%d\n", var.green.offset, var.green.length, var.green.msb_right);
	printf("blue: offset=%d length=%d msb_right=%d\n", var.blue.offset, var.blue.length, var.blue.msb_right);
	printf("x_offset: %d\n", var.xoffset);
	printf("y_offset: %d\n", var.yoffset);

	if(ioctl(fh, FBIOGET_FSCREENINFO, &fix))
	{
		printf("get fb fix info err\n");
		return;
	}
	printf("length of line in bytes: %d\n", fix.line_length);
	printf("(line_length * 8) / bits_per_pixel = %d\n", (fix.line_length*8)/var.bits_per_pixel);
	printf("memory length: %d\n", fix.smem_len);
	printf("%d\n", var.xres * var.yres * var.bits_per_pixel / 8);

	// draw rectangles to test frame buffer
	void* frameBuffer = (void *) mmap (
		0, fix.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fh, 0);
	drawRect(var.xres / 8, var.yres / 8, var.xres / 4, var.yres / 4, 0xffff0000,
             &var, &fix, frameBuffer);
	drawRect(var.xres * 3 / 8, var.yres * 3 / 8, var.xres / 4, var.yres / 4, 0xff00ff00,
             &var, &fix, frameBuffer);
	drawRect(var.xres * 5 / 8, var.yres * 5 / 8, var.xres / 4, var.yres / 4, 0xff0000ff,
             &var, &fix, frameBuffer);
	sleep(3);
	// clean screen
	printf("\033[H\033[J");
	fflush(stdout);
	close(fh);
}

int main()
{
	print_fb_info();
	return 0;
}

