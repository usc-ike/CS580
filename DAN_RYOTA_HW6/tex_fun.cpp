/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"

GzColor	*image=NULL;
int xs, ys;
int reset = 1;

static float Clampf(float num, float low, float up) {
	return max(low, min(num, up));
}

int GetArray(int x, int y) {
	return (x + y*xs);
}

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
  unsigned char		pixel[3];
  unsigned char     dummy;
  char  		foo[8];
  int   		i, j;
  FILE			*fd;

  if (reset) {          /* open and load texture file */
    fd = fopen ("texture", "rb");
    if (fd == NULL) {
      fprintf (stderr, "texture file not found\n");
      exit(-1);
    }
    fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
    image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));
    if (image == NULL) {
      fprintf (stderr, "malloc for texture image failed\n");
      exit(-1);
    }

    for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
      fread(pixel, sizeof(pixel), 1, fd);
      image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
      image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
      image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
      }

    reset = 0;          /* init is done */
	fclose(fd);
  }

/* bounds-test u,v to make sure nothing will overflow image array bounds */
  u = Clampf(u, 0, 1);
  v = Clampf(v, 0, 1);
/* determine texture cell corner values and perform bilinear interpolation */
  float s = u * (xs - 1.0);
  float t = v * (ys - 1.0);
  int abcd[4] = {
	  GetArray(floor(s), floor(t)),
	  GetArray(ceil(s), floor(t)),
	  GetArray(ceil(s), ceil(t)),
	  GetArray(floor(s), ceil(t))
  };
  s -= floor(s);
  t -= floor(t);
/* set color to interpolated GzColor value and return */
  for (i = 0; i < 3; ++i) {
	  color[i] = s* t * image[abcd[2]][i] + (1.0 - s) * t * image[abcd[3]][i] + s * (1.0 - t) * image[abcd[1]][i] + (1.0 - s)*(1.0 - t)*image[abcd[0]][i];
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

