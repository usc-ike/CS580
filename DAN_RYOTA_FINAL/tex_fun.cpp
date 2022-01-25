/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"

GzColor	*image=NULL;
GzColor *normalMap = NULL;
int xs, ys, xm, ym;

static float Clampf(float num, float low, float up) {
	return max(low, min(num, up));
}

int GetArray(int x, int y, int bounds) {
	return (x + y*bounds);
}

GzColor* parseMap(GzColor* texture, const char* file, int* x, int* y) {
	unsigned char pixel[3];
	unsigned char dummy;
	char foo[8];
	FILE *fd;

	fd = fopen(file, "rb");
	if (fd == NULL) {
		fprintf(stderr, "texture file not found\n");
		exit(-1);
	}
	fscanf(fd, "%s %d %d %c", foo, x, y, &dummy);
	texture = (GzColor*)malloc(sizeof(GzColor)*(*x + 1)*(*y + 1));
	if (texture == NULL) {
		fprintf(stderr, "malloc for texture image failed\n");
		exit(-1);
	}
	for (int i = 0; i < (*x) * (*y); ++i) {
		fread(pixel, sizeof(pixel), 1, fd);
		texture[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
		texture[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
		texture[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
	}
	fclose(fd);
	return texture;
}

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
	static bool reset = false;
	if (!reset) {
		image = parseMap(image, "texture", &xs, &ys);
		reset = true;
	}

/* bounds-test u,v to make sure nothing will overflow image array bounds */
  u = Clampf(u, 0, 1);
  v = Clampf(v, 0, 1);
/* determine texture cell corner values and perform bilinear interpolation */
  float s = u * (xs - 1.0);
  float t = v * (ys - 1.0);
  int abcd[4] = {
	  GetArray(floor(s), floor(t), xs),
	  GetArray(ceil(s), floor(t), xs),
	  GetArray(ceil(s), ceil(t), xs),
	  GetArray(floor(s), ceil(t), xs)
  };
  s -= floor(s);
  t -= floor(t);
/* set color to interpolated GzColor value and return */
  for (int i = 0; i < 3; ++i) {
	  color[i] = s* t * image[abcd[2]][i] + (1.0 - s) * t * image[abcd[3]][i] + s * (1.0 - t) * image[abcd[1]][i] + (1.0 - s)*(1.0 - t)*image[abcd[0]][i];
  }
  
	return GZ_SUCCESS;
}

int no_fun(float u, float v, GzColor normal) {
	static bool reset = false;
	if (!reset) {
		normalMap = parseMap(normalMap, "normalMap.ppm", &xm, &ym);
		reset = true;
	}

	/* bounds-test u,v to make sure nothing will overflow image array bounds */
	u = Clampf(u, 0, 1);
	v = Clampf(v, 0, 1);
	/* determine texture cell corner values and perform bilinear interpolation */
	float s = u * (xm - 1.0);
	float t = v * (ym - 1.0);
	int abcd[4] = {
		GetArray(floor(s), floor(t), xm),
		GetArray(ceil(s), floor(t), xm),
		GetArray(ceil(s), ceil(t), xm),
		GetArray(floor(s), ceil(t), xm)
	};
	s -= floor(s);
	t -= floor(t);
	/* set color to interpolated GzColor value and return */
	for (int i = 0; i < 3; ++i) {
		normal[i] = s* t * normalMap[abcd[2]][i] + (1.0 - s) * t * normalMap[abcd[3]][i] + s * (1.0 - t) * normalMap[abcd[1]][i] + (1.0 - s)*(1.0 - t)*normalMap[abcd[0]][i];
	}

	return GZ_SUCCESS;
}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
#define mode 0
#if mode == 0
	int interval = 6;
	int umod = u * interval;
	int vmod = v * interval;

	bool same = vmod % 2 == umod % 2;

	for (int i = 0; i < 3; ++i) {
		color[i] = same ? 0 : 4095;
	}
#elif mode == 1

	
#else

#endif

	return GZ_SUCCESS;
}

/* Free texture memory */
int GzFreeTexture()
{
	if(image!=NULL)
		free(image);
	return GZ_SUCCESS;
}

