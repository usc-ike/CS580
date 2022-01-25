#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
/*   CS580 HW   */
#include    "stdafx.h"  
#include	"Gz.h"


GzRender::GzRender(int xRes, int yRes)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- set display resolution
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- allocate memory for pixel buffer
 */
	xres = xRes;
	yres = yRes;

	framebuffer = new char[3 * xRes * yRes];
	pixelbuffer = new GzPixel[xRes * yRes];
}

GzRender::~GzRender()
{
/* HW1.2 clean up, free buffer memory */
	if (framebuffer != NULL) {
		delete framebuffer;
	}
	if (pixelbuffer != NULL) {
		delete pixelbuffer;
	}

}

int GzRender::GzDefault()
{
/* HW1.3 set pixel buffer to some default values - start a new frame */
	for (int i = 0; i < xres * yres; i++) {
		pixelbuffer[i].red = 200<<4;
		pixelbuffer[i].blue = 200<<4;
		pixelbuffer[i].green = 0<<4;
		pixelbuffer[i].alpha = 1;
		pixelbuffer[i].z = 0;
	}
	return GZ_SUCCESS;
}


int GzRender::GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
	if (i < 0 || i >= xres) return GZ_FAILURE;
	if (j < 0 || j >= yres) return GZ_FAILURE;
/* HW1.4 write pixel values into the buffer */
	int idx = ARRAY(i, j);
	pixelbuffer[idx].red = r;
	pixelbuffer[idx].blue = b;
	pixelbuffer[idx].green = g;
	pixelbuffer[idx].alpha = a;
	pixelbuffer[idx].z = z;
	return GZ_SUCCESS;
}


int GzRender::GzGet(int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
	if (i < 0 || i >= xres) return GZ_FAILURE;
	if (j < 0 || j >= yres) return GZ_FAILURE;
/* HW1.5 retrieve a pixel information from the pixel buffer */
	int idx = ARRAY(i, j);
	*r = pixelbuffer[idx].red;
	*b = pixelbuffer[idx].blue;
	*g = pixelbuffer[idx].green;
	*a = pixelbuffer[idx].alpha;
	*z = pixelbuffer[idx].z;
	return GZ_SUCCESS;
}

int Clamp(int num, int low, int up) {
	return max(low, min(num, up));
}

int GzRender::GzFlushDisplay2File(FILE* outfile)
{
/* HW1.6 write image to ppm file -- "P6 %d %d 255\r" */
	fprintf(outfile, "P6 %d %d 255\n", xres, yres);
	for (int i = 0; i < xres * yres; i++) {
		int red = Clamp(pixelbuffer[i].red, 0, 4095);
		int blue = Clamp(pixelbuffer[i].blue, 0, 4095);
		int green = Clamp(pixelbuffer[i].green, 0, 4095);
		red = red >> 4;
		blue = blue >> 4;
		green = green >> 4;
		fprintf(outfile, "%c%c%c", red, green, blue);
	}
	
	return GZ_SUCCESS;
}

int GzRender::GzFlushDisplay2FrameBuffer()
{
/* HW1.7 write pixels to framebuffer: 
	- put the pixels into the frame buffer
	- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red 
	- NOT red, green, and blue !!!
*/
	for (int i = 0; i < xres * yres; i++) {
		int red = Clamp(pixelbuffer[i].red, 0, 4095);
		int blue = Clamp(pixelbuffer[i].blue, 0, 4095);
		int green = Clamp(pixelbuffer[i].green, 0, 4095);
		red = red >> 4;
		blue = blue >> 4;
		green = green >> 4;
		framebuffer[i * 3] = blue;
		framebuffer[i * 3 + 1] = green;
		framebuffer[i * 3 + 2] = red;
	}
	return GZ_SUCCESS;
}