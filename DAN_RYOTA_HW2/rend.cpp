#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"



/***********************************************/
/* HW1 methods: copy here the methods from HW1 */

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
		pixelbuffer[i].red = 200 << 4;
		pixelbuffer[i].blue = 200 << 4;
		pixelbuffer[i].green = 200 << 4;
		pixelbuffer[i].alpha = 1;
		pixelbuffer[i].z = MAXINT;
	}
	return GZ_SUCCESS;
}


int GzRender::GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* HW1.4 write pixel values into the buffer */
	if (i < 0 || i >= xres) return GZ_FAILURE;
	if (j < 0 || j >= yres) return GZ_FAILURE;
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
/* HW1.5 retrieve a pixel information from the pixel buffer */
	if (i < 0 || i >= xres) return GZ_FAILURE;
	if (j < 0 || j >= yres) return GZ_FAILURE;
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


/***********************************************/
/* HW2 methods: implement from here */

int GzRender::GzPutAttribute(int numAttributes, GzToken	*nameList, GzPointer *valueList) 
{
/* HW 2.1
-- Set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
-- In later homeworks set shaders, interpolaters, texture maps, and lights
*/
	for (int i = 0; i < numAttributes; i++) {
		switch (nameList[i]) {
			case GZ_RGB_COLOR:
				//printf("%f %f %f\n", valueList[0], valueList[1], valueList[2]);
				//GzColor* values = static_cast<GzColor*>(valueList[i]);
				//printf("%f %f %f\n", values[0], values[1], values[2]);
				float* color = (float*)valueList[i];
				for (int j = 0; j < 3; j++) {
					//flatcolor[j] = *(float*)&valueList[j];
					flatcolor[j] = color[j];
				}
				//printf("%f %f %f\n", flatcolor[0], flatcolor[1], flatcolor[2]);
				break;
		}
	}
	return GZ_SUCCESS;
}

class GZ_DDA {
public:
	GzCoord start, end, current;
	float slopeX, slopeZ;
public:
	GZ_DDA() {

	}
	GZ_DDA(GzCoord start, GzCoord end) {
		if (start[1] > end[1]) {
			for (int i = 0; i < 3; i++) {
				this->start[i] = end[i];
				this->end[i] = start[i];
				this->current[i] = end[i];
			}
		} else {
			for (int i = 0; i < 3; i++) {
				this->start[i] = start[i];
				this->end[i] = end[i];
				this->current[i] = start[i];
			}
		}
		
		if (end[1] - start[1] == 0) {
			slopeX = NULL;
			slopeZ = NULL;
			return;
		}

		slopeX = (end[0] - start[0]) / (end[1] - start[1]);
		slopeZ = (end[2] - start[2]) / (end[1] - start[1]);
	}

	void Print(bool details = false) {
		printf("Start: %f %f %f\n", start[0], start[1], start[2]);
		printf("End: %f %f %f\n", end[0], end[1], end[2]);
		printf("SlopeX: %f, SlopeZ: %f\n", slopeX, slopeZ);
		if (details) {
			printf("Current: %f %f %f\n", current[0], current[1], current[2]);
		}
		printf("------------------\n");
	}

	void SetCurrent(GzCoord coord) {
		for (int i = 0; i < 3; i++) {
			current[i] = coord[i];
		}
	}

	void ShiftCurrent(float dY) {
		current[0] += slopeX * dY;
		current[1] += dY;
		current[2] += slopeZ * dY;
	}
};
typedef GZ_DDA DDAs[3];

class SPAN {
public :
	GzCoord start;
	GzCoord end;
	GzCoord current;
	float slopeZ;
public:
	SPAN(){}
	SPAN(GzCoord start, GzCoord end) {
		for (int i = 0; i < 3; i++) {
			this->start[i] = start[i];
			this->end[i] = end[i];
			this->current[i] = start[i];
		}
		//dz/dx  (RZ-LZ)/(RX-LX)
		slopeZ = (end[2] - start[2]) / (end[0] - start[0]);
	}

	void ShiftCurrent(float dX) {
		current[0] += dX;
		current[2] += slopeZ * dX;
	}

	void Print(bool details = false) {
		printf("SPAN Start: %f %f %f\n", start[0], start[1], start[2]);
		printf("SPAN End: %f %f %f\n", end[0], end[1], end[2]);
		if (details) {
			printf("SPAN Current: %f %f %f\n", current[0], current[1], current[2]);
		}
	}
};

float GetDelta(float num) {
	float res = ceilf(num) - num;
	return res;
}

int CompareCoord(GzCoord a, GzCoord b) {
	return 0;
}

void CrossProduct(GzCoord a, GzCoord b, GzCoord out) {
	out[0] = a[1] * b[2] - a[2] * b[1];
	out[1] = a[2] * b[0] - a[0] * b[2];
	out[2] = a[0] * b[1] - a[1] * b[0];
}

void Debug() {
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
}

int GzRender::GzPutTriangle(int	numParts, GzToken *nameList, GzPointer *valueList) 
/* numParts - how many names and values */
{
/* HW 2.2
-- Pass in a triangle description with tokens and values corresponding to
      GZ_NULL_TOKEN:		do nothing - no values
      GZ_POSITION:		3 vert positions in model space
-- Invoke the rastrizer/scanline framework
-- Return error code
*/
	Debug();
	//printf("%d\n", MAXINT);
	/*GzCoord* temp = static_cast<GzCoord*>(valueList[0]);
	printf("%f %f %f\n", temp[0][0], temp[0][1], temp[0][2]);
	printf("%f %f %f\n", temp[1][0], temp[1][1], temp[1][2]);*/
	//float** temp = (float**)valueList;
	//printf("%f %f %f\n", temp[0][0], temp[0][1], temp[0][2]);
	//printf("%f %f %f\n", temp[1][0], temp[1][1], temp[1][2]);

	for (int i = 0; i < numParts; i++) {
		switch (nameList[i]) {
			case GZ_POSITION:
				//2.2.1~2.2.2
				DDAs data;
				GzCoord* coords = static_cast<GzCoord*>(valueList[i]);
				int temp[3] = { 0,1,2 };
				for (int j = 0; j < 3; j++) {
					for (int k = j + 1; k < 3; k++) {
						if (coords[temp[j]][1] > coords[temp[k]][1]) {
							int tv = temp[j];
							temp[j] = temp[k];
							temp[k] = tv;
							k--;
							continue;
						} else if (coords[temp[j]][1] == coords[temp[k]][1]) {
							if (coords[temp[j]][0] > coords[temp[k]][0]) {
								int tv = temp[j];
								temp[j] = temp[k];
								temp[k] = tv;
							}
						}
					}
				}
				
				for (int j = 0; j < 3; j++) {
					data[j] = GZ_DDA(coords[temp[j]], coords[temp[(j + 1) % 3]]);
					//data[j].Print(true);
				}
				//2.2.3~2.2.4
				GZ_DDA* left[3];
				GZ_DDA* right[3];
				for (int j = 0; j < 3; j++) {
					left[j] = NULL;
					right[j] = NULL;
				}
				//Check if triangle has horizontal base
				if (data[1].slopeX == NULL) {
					left[0] = &data[0];
					right[0] = &data[2];
				} else {
					//formulate equation of 1-3
					//Ax + By + C = 0
					//(y1 – y2)x + (x2 – x1)y + (x1y2 – x2y1) = 0
					float pt = (-(data[2].end[0] - data[2].start[0]) * data[0].end[1] - (data[2].start[0] * data[2].end[1] - data[2].end[0] * data[2].start[1])) / (data[2].start[1] - data[2].end[1]);
					//printf("%f <==> %f\n", pt, data[0].end[0]);
					//system("PAUSE");
					//yInt = A.y - A.x*1-3.slopeX
					//printf("%f\n", (data[2].end[1] - data[2].start[1]) / (data[2].end[0] - data[2].start[0]));
					//float yIntercept = data[2].start[1] - data[2].start[0] * data[2].slopeX;
					//point = (B.y-yInt)/1-3.slopeX
					//x = (y-b)/a
					//float point = (data[0].end[1] - yIntercept) / data[2].slopeX;
					//The lesser x is to the left
					//printf("%f\n", pt);
					//printf("%f, %f <=> %f, %f\n", data[0].end[0], data[0].end[1], point, data[2].slopeX * point + yIntercept);
					//printf("%f\n", data[2].slopeX * data[2].end[0] + yIntercept);
					if (pt < data[0].end[0]) {
						left[0] = &data[2];
						right[0] = &data[0];
						right[1] = &data[1];
						//printf("<<<<<<\n");
					} else {
						left[0] = &data[0];
						left[1] = &data[1];
						right[0] = &data[2];
						//printf(">>>>>>>>>>\n");
					}
				}
				/*for (int j = 0; j < 2; j++) {
					if(left[j]!= NULL)
						left[j]->Print();
				}
				printf("L----------------R\n");
				for (int j = 0; j < 2; j++) {
					if (right[j] != NULL)
						right[j]->Print();
				}*/
				//2.2.5
				left[0]->SetCurrent(data[0].start);
				left[0]->ShiftCurrent(GetDelta(left[0]->current[1]));
				right[0]->SetCurrent(data[0].start);
				right[0]->ShiftCurrent(GetDelta(right[0]->current[1]));
				//2.2.6
				GZ_DDA** curLeft = left;
				GZ_DDA** curRight = right;
				/*printf("%x\n%x\n", &left[0], &left[1]);
				printf("%x\n%x\n", &right[0], &right[1]);
				/*(*curRight)->Print(true);
				printf("%x\n", &*curRight);
				curRight++;
				printf("%x\n", &*curRight);
				(*curRight)->Print();*/
				for (;;(*curLeft)->ShiftCurrent(1), (*curRight)->ShiftCurrent(1)) {
					if ((*curLeft)->current[1] > (*curLeft)->end[1]) {
						curLeft++;
						if (*curLeft == NULL) break;
						(*curLeft)->ShiftCurrent(GetDelta((*curLeft)->current[1]));
					}
					
					if ((*curRight)->current[1] > (*curRight)->end[1]) {
						//printf("%f <> %f\n", (*curRight)->current[1], (*curRight)->end[1]);
						curRight++;
						if (*curRight == NULL) break;
						(*curRight)->ShiftCurrent(GetDelta((*curRight)->current[1]));
					}
					//setup span
					//printf("Spanning\n");
					SPAN span = SPAN((*curLeft)->current, (*curRight)->current);
					span.ShiftCurrent(GetDelta(span.current[0]));
					//span.Print(true);
					for (; span.current[0] <= span.end[0]; span.ShiftCurrent(1)) {
						if (span.current[0] >= 0 && span.current[1] >= 0 && span.current[0] < xres && span.current[1] < yres) {
						int pixelIdx = ARRAY(span.current[0], span.current[1]);
						if (pixelIdx < 0 || pixelIdx >= xres * yres) continue;
							//printf("%f, %f\n", span.current[0], span.current[1]);
							//printf("%f <==> %d\n", span.current[2], pixelbuffer[pixelIdx].z);
							if (span.current[2] < pixelbuffer[pixelIdx].z) {
								//printf("Changed\n");
								pixelbuffer[pixelIdx].blue = Clamp(ctoi(flatcolor[0]), 0, 4095);
								pixelbuffer[pixelIdx].green = Clamp(ctoi(flatcolor[1]), 0, 4095);
								pixelbuffer[pixelIdx].red = Clamp(ctoi(flatcolor[2]), 0, 4095);
								pixelbuffer[pixelIdx].z = span.current[2];
								
								//printf("--------\n%d\n%d\n%d\n%d\n", pixelbuffer[pixelIdx].blue, pixelbuffer[pixelIdx].green, pixelbuffer[pixelIdx].red, pixelbuffer[pixelIdx].z);
								

							}
						} else {
							//printf("%f %f\n", span.current[0], span.current[1]);
						}
					}
					
				}

				//printf("---------\n");
				//system("PAUSE");
				break;
		}
	}
	return GZ_SUCCESS;
}

