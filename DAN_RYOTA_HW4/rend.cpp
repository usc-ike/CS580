/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

#define PI (float) 3.14159265358979323846

#pragma region Tools



#pragma region Replace
float* Replace(float destination[3], float source[3]) {
	for (int i = 0; i < 3; ++i) {
		destination[i] = source[i];
	}
	return destination;
}


void Replace(GzMatrix destination, GzMatrix source) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			destination[i][j] = source[i][j];
		}
	}
}

GzLight* Replace(GzLight* destination, GzLight* source) {
	Replace(destination->color, source->color);
	Replace(destination->direction, source->direction);
	return destination;
}

GzLight* Replace(GzLight* destination, GzLight source) {
	return Replace(destination, &source);
}

void Replace(GzMatrix destination, GzCoord* source) {
	for (int i = 0; i < 3; i++) {
		Replace(destination[i], source[i]);
		destination[i][3] = 0;
		destination[3][i] = 0;
	}
	destination[3][3] = 1;
}

GzCoord* Replace(GzCoord* destination, GzMatrix source) {
	for (int i = 0; i < 3; ++i) {
		Replace(destination[i], source[i]);
	}
	return destination;
}
#pragma endregion


#pragma region Multiply
void Multiply(GzMatrix mat, GzMatrix mat2, GzMatrix result) {
	GzMatrix res;
	float resF;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			resF = 0;
			for (int k = 0; k < 4; ++k) {
				resF += mat[i][k] * mat2[k][j];
			}
			res[i][j] = resF;
		}
	}
	Replace(result, res);
}

void Multiply(GzMatrix mat, GzMatrix mat2) {
	Multiply(mat, mat2, mat);
}

float* Multiply(GzCoord a, float b, GzCoord out) {
	for (int i = 0; i < 3; i++) {
		out[i] = a[i] * b;
	}
	return out;
}

float* Multiply(GzCoord a, float b) {
	return Multiply(a, b, a);
}

float* Multiply(GzMatrix mat, GzCoord coo, float* out) {
	float temp[4];
	float res[4];
	float resF;
	for (int i = 0; i < 3; ++i) {
		temp[i] = coo[i];
	}
	temp[3] = 1;

	for (int i = 0; i < 4; ++i) {
		resF = 0;
		for (int j = 0; j < 4; ++j) {
			resF += mat[i][j] * temp[j];
		}
		res[i] = resF;
	}

	for (int i = 0; i < 4; i++) {
		out[i] = res[i];
	}
	return out;

}
#pragma endregion

#pragma region Print

class Debug {
public:
	static bool Initialize() {
		static bool initialized = false;
		if (!initialized) {
			AllocConsole();
			freopen("CONOUT$", "w", stdout);
			initialized = true;
		}
		return initialized;
	}

	static void LogFormat(const char* _Format, ...) {
		static bool initialized = false;
		if (!initialized) {
			initialized = Initialize();
		}
		va_list args;
		va_start(args, _Format);
		vprintf(_Format, args);
		va_end(args);
	}

	static void Log(const char* str, bool endLine = true) {
		static bool initialized = false;
		if (!initialized) {
			initialized = Initialize();
		}
		printf(str);
		if (endLine) EndLine();
	}

	//Prints out 1 new line
	static void EndLine() {
		static bool initialized = false;
		if (!initialized) {
			initialized = Initialize();
		}
		printf("\n");
	}

	//Prints out any amt of new lines
	//Default: 1
	static void NewLine(int times = 1) {
		for (int i = 0; i < times; ++i) {
			EndLine();
		}
	}

	//Mainly for printing GzCoord/GzColor
	//Can print out any float array
	static void Log(float* vector, int length = 3) {
		for (int i = 0; i < length; ++i) {
			LogFormat("%.8f ", vector[i]);
		}
		EndLine();
	}
	
	//Prints out GzLight
	static void Log(GzLight light) {
		Log("Direction: ", false);
		Log(light.direction);
		Log("Color: ", false);
		Log(light.color);
	}

	//Print out array of GzCoords
	static void Log(GzCoord* coords, int length = 3) {
		for (int i = 0; i < length; ++i) {
			Log(coords[i]);
		}
	}

	//Prints out a GzMatrix
	static void Log(GzMatrix mat, bool endLine = true) {
		for (int i = 0; i < 4; ++i) {
			Log(mat[i]);
		}
		if (endLine) EndLine();
	}

	//For printing any sized 2 dimensional float arrays
	//Default: 4x4
	static void Log(float** data, int length = 4, int width = 4, bool endLine = true) {
		for (int i = 0; i < length; ++i) {
			Log(data[i], width);
		}
		if (endLine) EndLine();
	}

	static void LogStack(GzMatrix stack[MATLEVELS], int matLevel, GzMatrix extra = NULL) {
		Log("Stack:");
		for (int i = 0; i < matLevel + 1; ++i) {
			LogFormat("Level; %d\n", i);
			Log(stack[i]);
		}
		if (extra != NULL) {
			Log(extra);
		}
		Log("End of Stack");
	}

	static void LogStack(GzMatrix stack[MATLEVELS], int matLevel, GzCoord* extra) {
		GzMatrix temp;
		Replace(temp, extra);
		LogStack(stack, matLevel, temp);
	}

};

#pragma endregion

#pragma region Others
int AddLight(GzRender* rend, GzLight light) {
	if (rend->numlights < 0) rend->numlights = 0;
	if (rend->numlights >= 10) {
		printf("Max number of lights reached\n");
		return GZ_FAILURE;
	}
	rend->lights[rend->numlights++] = light;
	return GZ_SUCCESS;
}

int AddLight(GzRender* rend, GzLight* light) {
	return AddLight(rend, *light);
}

float ToRadian(float degree) {
	return degree * (PI / 180);
}


float Dot(GzCoord c1, GzCoord c2) {
	return c1[0] * c2[0] + c1[1] * c2[1] + c1[2] * c2[2];
}

float* Cross(GzCoord a, GzCoord b, GzCoord out) {
	out[0] = a[1] * b[2] - a[2] * b[1];
	out[1] = a[2] * b[0] - a[0] * b[2];
	out[2] = a[0] * b[1] - a[1] * b[0];
	return out;
}

float* Cross(GzCoord a, GzCoord b) {
	return Cross(a, b, a);
}


float* Add(GzCoord a, GzCoord b, GzCoord out) {
	for (int i = 0; i < 3; i++) {
		out[i] = a[i] + b[i];
	}
	return out;
}

float* Add(GzCoord a, GzCoord b) {
	return Add(a, b, a);
}

float* Subtract(GzCoord a, GzCoord b, GzCoord out) {
	for (int i = 0; i < 3; i++) {
		out[i] = a[i] - b[i];
	}
	return out;
}

float* Subtract(GzCoord a, GzCoord b) {
	return Subtract(a, b, a);
}

float* Normalize(GzCoord coord) {
	float magnitude = coord[0] * coord[0] + coord[1] * coord[1] + coord[2] * coord[2];
	magnitude = sqrtf(magnitude);
	for (int i = 0; i < 3; i++) {
		coord[i] /= magnitude;
	}
	return coord;
}

float Magnitude(float* coord) {
	return sqrt(coord[0] * coord[0] + coord[1] * coord[1] + coord[2] * coord[2]);
}

float Clampf(float num, float low, float up) {
	return max(low, min(num, up));
}

int Clamp(int num, int low, int up) {
	return max(low, min(num, up));
}

float GetDelta(float num) {
	float res = ceilf(num) - num;
	return res;
}

void Uniform(GzMatrix mat) {
	//static int runCount;
	//runCount++;
	//printf("Runcount: %d\n", runCount);
	for (int i = 0; i < 3; i++) {
		mat[i][3] = 0;
	}
	mat[3][3] = 1;
	//printf("Before: \n");
	//Print(mat);
#if 1
	float k = 1 / sqrt(mat[0][0] * mat[0][0] + mat[0][1] * mat[0][1] + mat[0][2] * mat[0][2] + mat[0][3] * mat[0][3]);
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			mat[i][j] *= k;
		}
	}
#else
	///////////////////////
	float scale[3];
	for (int i = 0; i < 3; ++i) {
		scale[i] = 1.0f / sqrt(mat[0][i] * mat[0][i] + mat[1][i] * mat[1][i] + mat[2][i] * mat[2][i]);
	}
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			mat[i][j] *= scale[j];
		}
	}
#endif
	/*for (int i = 0; i < 3; i++) {
		mat[i][3] = 0;
	}
	mat[3][3] = 1;*/
	//PrintNew();
	//printf("\nAfter:\n");
	//Print(mat);
	//PrintNew(3);
}

float getZInterp(float a, float b, float c, float d, float x, float y) {
	return((-(a*x) - (b*y) - d) / c);
}

void ToRGB(GzColor bgr) {
	float temp = bgr[0];
	bgr[0] = bgr[2];
	bgr[2] = temp;
}

void ToBGR(GzColor rgb) {
	float temp = rgb[0];
	rgb[0] = rgb[2];
	rgb[2] = temp;
}
#pragma endregion

#pragma endregion

#pragma region Classes
class GZ_DDA {
public:
	GzCoord start, end, current;
	GzCoord startN, endN, currentN;
	float slopeX, slopeZ;
public:
	GZ_DDA() {

	}
	GZ_DDA(GzCoord start, GzCoord end, GzCoord startN = NULL, GzCoord endN = NULL) {

		if (start[1] > end[1]) {
			for (int i = 0; i < 3; i++) {
				this->start[i] = end[i];
				this->end[i] = start[i];
				this->current[i] = end[i];
				/////////////////////////
				if (startN != NULL && endN != NULL) {
					this->startN[i] = endN[i];
					this->endN[i] = startN[i];
					this->currentN[i] = endN[i];
				}
				
			}
		}
		else {
			for (int i = 0; i < 3; i++) {
				this->start[i] = start[i];
				this->end[i] = end[i];
				this->current[i] = start[i];
				////////////////////
				if (startN != NULL && endN != NULL) {
					this->startN[i] = startN[i];
					this->endN[i] = endN[i];
					this->currentN[i] = startN[i];
				}
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
public:
	GzCoord start;
	GzCoord end;
	GzCoord current;
	float slopeZ;
public:
	SPAN() {}
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

#pragma endregion

#pragma region Functions
void Colorize(GzRender* rend, GzColor out, GzCoord N) {
	/*
	C = (Ks Sig_l(le(R . E)^s))  specular
	+ (Kd Sig_l(le(N . L))) diffuse
	+ (Ka * la) ambient

	C = color at particular point
	GzColor Ks = coefficient of specular light (RGB color) [0,1]
	GzColor Kd = coefficient of diffuse
	GzColor Ka = coefficient of ambient
	GzLight->GzColor le = color of directional light
	GzLight->GzColor la = color of ambient light
	GzCoord N = normal of vertex
	given in model space
	GzLight->GzCoord L = direction of light
	constant
	GzCoord R = reflection vector of light
	R = 2(N . L)N-L
	GzCoord E = direction of light to camera
	Lookat point
	constant
	float s = specular power. controls the distribution of energy about R (given)
	*/


	GzCoord E = { 0, 0, -1 };
	//Subtract(rend->m_camera.lookat, rend->m_camera.position, E);
	//Replace(E, rend->m_camera.lookat);
	Normalize(E);
	//Print(E);

	GzCoord* R = new GzCoord[rend->numlights];

	float s = rend->spec;

	int* sign = new int[rend->numlights];

	float ne = Dot(N, E);
	//Debug::LogFormat("NE: %f\n", ne);
	for (int i = 0; i < rend->numlights; ++i) {
		float nl = Dot(N, rend->lights[i].direction);
		//Debug::Log(N);
		//Debug::LogFormat("NL%d: %f\n", i, nl);
		if (ne > 0 && nl > 0) {
			sign[i] = 1;
		}
		else if (ne < 0 && nl < 0) {
			sign[i] = -1;

		}
		else {
			sign[i] = 0;
			continue;
		}

		Multiply(N, sign[i], R[i]);
		Multiply(R[i], 2 * nl);
		Subtract(R[i], rend->lights[i].direction, R[i]);
		Normalize(R[i]);
		//Debug::Log(R[i]);
	}

	//Specular (Ks Sig_l(le(R . E)^s))
	GzColor sColor = { 0, 0, 0 };
	for (int i = 0; i < rend->numlights; ++i) {
		if (sign[i] == 0) continue;
		GzColor temp;
		//float dbg = Clampf(Dot(R[i], E), 0, 1);
		//Debug::LogFormat("R for Light %d: ", i);
		//Debug::Log(R[i]);
		//Debug::LogFormat("Dot of R%d and E: %f\n", i, dbg);
		Multiply(rend->lights[i].color, pow(Clampf(Dot(R[i], E), 0, 1), s), temp);
		//printf("temp:\n");
		//Print(temp);
		Add(sColor, temp, sColor);
	}
	for (int i = 0; i < 3; ++i) {
		sColor[i] *= rend->Ks[i];
	}

	//Diffuse (Kd Sig_l(le(N . L)))
	GzColor dColor = { 0,0,0 };
	for (int i = 0; i < rend->numlights; ++i) {
		if (sign[i] == 0) continue;
		GzColor temp;
		Multiply(N, sign[i], temp);
		Multiply(rend->lights[i].color, Dot(temp, rend->lights[i].direction), temp);
		Add(dColor, temp, dColor);
	}
	for (int i = 0; i < 3; ++i) {
		dColor[i] *= rend->Kd[i];
	}

	//Ambient (Ka * la)
	GzColor aColor;
	for (int i = 0; i < 3; ++i) {
		aColor[i] = rend->Ka[i] * rend->ambientlight.color[i];
	}
	//Print(sColor);
	//Print(dColor);
	//Print(aColor);

	Add(sColor, dColor, out);
	Add(out, aColor, out);
}

void ScanLine(GzRender* rend, GzCoord* coords, GzCoord* coordsN) {
	//PrintStack(rend->Xnorm, rend->matlevel);
	DDAs data;
	int temp[3] = { 0,1,2 };
	for (int j = 0; j < 3; j++) {
		for (int k = j + 1; k < 3; k++) {
			if (coords[temp[j]][1] > coords[temp[k]][1]) {
				int tv = temp[j];
				temp[j] = temp[k];
				temp[k] = tv;
				k--;
				continue;
			}
			else if (coords[temp[j]][1] == coords[temp[k]][1]) {
				if (coords[temp[j]][0] > coords[temp[k]][0]) {
					int tv = temp[j];
					temp[j] = temp[k];
					temp[k] = tv;
				}
			}
		}
	}

	for (int j = 0; j < 3; j++) {
		data[j] = GZ_DDA(coords[temp[j]], coords[temp[(j + 1) % 3]], coordsN == NULL ? NULL : coordsN[temp[j]], coordsN == NULL ? NULL : coordsN[temp[(j + 1) % 3]]);
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
	}
	else {
		//formulate equation of 1-3
		//Ax + By + C = 0
		//(y1 – y2)x + (x2 – x1)y + (x1y2 – x2y1) = 0
		//pt = x
		float pt = (-(data[2].end[0] - data[2].start[0]) * data[0].end[1] - (data[2].start[0] * data[2].end[1] - data[2].end[0] * data[2].start[1])) / (data[2].start[1] - data[2].end[1]);
		if (pt < data[0].end[0]) {
			left[0] = &data[2];
			right[0] = &data[0];
			right[1] = &data[1];
		}
		else {
			left[0] = &data[0];
			left[1] = &data[1];
			right[0] = &data[2];
		}
	}

	//2.2.5
	left[0]->SetCurrent(data[0].start);
	left[0]->ShiftCurrent(GetDelta(left[0]->current[1]));
	right[0]->SetCurrent(data[0].start);
	right[0]->ShiftCurrent(GetDelta(right[0]->current[1]));
	//PrintStack(rend->Ximage, rend->matlevel);
	
	//2.2.6
	GZ_DDA** curLeft = left;
	GZ_DDA** curRight = right;

	//abcd[RGB][abcd]
	//abcd[n1n2n3[abcd]
	float abcd[3][4];
	
	
	if (rend->interp_mode != GZ_FLAT) {
		GzCoord v0, v1, planeConst;
		GzCoord cm[3];
		switch (rend->interp_mode) {
			case GZ_COLOR:
				GzColor cv[3];
				for (int i = 0; i < 3; ++i) {
					Colorize(rend, cv[i], coordsN[i]);
					Replace(cm[i], coords[i]);
				}

				for (int i = 0; i < 3; ++i) {
					for (int j = 0; j < 3; ++j) {
						cm[j][2] = cv[j][i];
					}
					Subtract(cm[1], cm[0], v0);
					Subtract(cm[2], cm[0], v1);
					Cross(v0, v1, planeConst);
					Replace(abcd[i], planeConst);
					abcd[i][3] = -(planeConst[0] * cm[0][0] + planeConst[1] * cm[0][1] + planeConst[2] * cm[0][2]);
				}
				break;
			case GZ_NORMALS:
				for (int i = 0; i < 3; ++i) {
					Replace(cm[i], coords[i]);
				}
				for (int i = 0; i < 3; ++i) {
					for (int j = 0; j < 3; ++j) {
						cm[j][2] = coordsN[j][i];
					}
					Subtract(cm[1], cm[0], v0);
					Subtract(cm[2], cm[0], v1);
					Cross(v0, v1, planeConst);
					Replace(abcd[i], planeConst);
					abcd[i][3] = -(planeConst[0] * cm[0][0] + planeConst[1] * cm[0][1] + planeConst[2] * cm[0][2]);
				}
				break;
		}
	}


	for (;; (*curLeft)->ShiftCurrent(1), (*curRight)->ShiftCurrent(1)) {
		if ((*curLeft)->current[1] > (*curLeft)->end[1]) {
			curLeft++;
			if (*curLeft == NULL) break;
			(*curLeft)->ShiftCurrent(GetDelta((*curLeft)->current[1]));
		}

		if ((*curRight)->current[1] > (*curRight)->end[1]) {
			curRight++;
			if (*curRight == NULL) break;
			(*curRight)->ShiftCurrent(GetDelta((*curRight)->current[1]));
		}

		SPAN span = SPAN((*curLeft)->current, (*curRight)->current);
		span.ShiftCurrent(GetDelta(span.current[0]));
		for (; span.current[0] <= span.end[0]; span.ShiftCurrent(1)) {
			if (span.current[0] >= 0 && span.current[1] >= 0 && span.current[0] < rend->xres && span.current[1] < rend->yres) {
				int pixelIdx = rend->ARRAY(span.current[0], span.current[1]);
				if (pixelIdx < 0 || pixelIdx >= rend->xres * rend->yres) continue;
				if (span.current[2] < rend->pixelbuffer[pixelIdx].z) {
					//In BGR
					GzColor fColor;
					switch (rend->interp_mode) {
						case GZ_FLAT:
							Colorize(rend, fColor, coordsN[0]);
							break;
						case GZ_COLOR:
							//Gouraud
							for (int i = 0; i < 3; ++i) {
								fColor[i] = getZInterp(abcd[i][0], abcd[i][1], abcd[i][2], abcd[i][3], span.current[0], span.current[1]);
								fColor[i] = fColor[i] > 1 ? 1.0f : fColor[i];
							}
							//Debug::Log(fColor);
							break;
						case GZ_NORMALS:
							//Phong
							GzCoord aNorm;
							for (int i = 0; i < 3; ++i) {
								aNorm[i] = getZInterp(abcd[i][0], abcd[i][1], abcd[i][2], abcd[i][3], span.current[0], span.current[1]);
							}
							Normalize(aNorm);
							Colorize(rend, fColor, aNorm);
							break;
					}
					ToBGR(fColor);
					rend->pixelbuffer[pixelIdx].blue = Clamp(rend->ctoi(fColor[0]), 0, 4095);
					rend->pixelbuffer[pixelIdx].green = Clamp(rend->ctoi(fColor[1]), 0, 4095);
					rend->pixelbuffer[pixelIdx].red = Clamp(rend->ctoi(fColor[2]), 0, 4095);
					rend->pixelbuffer[pixelIdx].z = span.current[2];
					
				}
			}
		}

	}
}

bool TransformVertex(GzMatrix mat, GzCoord* coords) {
	bool invalid = false;
	for (int i = 0; i < 3; ++i) {
		float temp[4];
		Multiply(mat, coords[i], temp);
		if (temp[Z] < 0) {
			invalid = true;
		}
		GzCoord rep;
		for (int j = 0; j < 3; j++) {
			rep[j] = temp[j] / temp[3];
		}
		Replace(coords[i], rep);
	}

	for (int i = 0; i < 3; ++i) {
		if (coords[i][Z] < 0) {
			invalid = true;
			break;
		}
	}
	return invalid;
}


#pragma endregion


int GzRender::GzRotXMat(float degree, GzMatrix mat)
{
/* HW 3.1
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
*/
	float rad = ToRadian(degree);
	GzMatrix rotX = {
		1, 0, 0, 0,
		0, cosf(rad), -sinf(rad), 0,
		0, sinf(rad), cosf(rad), 0,
		0, 0, 0, 1
	};
	Replace(mat, rotX);
	return GZ_SUCCESS;
}

int GzRender::GzRotYMat(float degree, GzMatrix mat)
{
/* HW 3.2
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
*/
	float rad = ToRadian(degree);
	GzMatrix rotY = {
		cosf(rad), 0, sinf(rad), 0,
		0, 1, 0, 0,
		-sinf(rad), 0, cosf(rad), 0,
		0, 0, 0, 1
	};
	Replace(mat, rotY);
	return GZ_SUCCESS;
}

int GzRender::GzRotZMat(float degree, GzMatrix mat)
{
/* HW 3.3
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
*/
	float rad = ToRadian(degree);
	GzMatrix rotZ = {
		cosf(rad), -sinf(rad), 0, 0,
		sinf(rad), cosf(rad), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	Replace(mat, rotZ);
	return GZ_SUCCESS;
}

int GzRender::GzTrxMat(GzCoord translate, GzMatrix mat)
{
/* HW 3.4
// Create translation matrix
// Pass back the matrix using mat value
*/
	GzMatrix trans = {
		1, 0, 0, translate[0],
		0, 1, 0, translate[1],
		0, 0, 1, translate[2],
		0, 0, 0, 1
	};
	Replace(mat, trans);
	return GZ_SUCCESS;
}


int GzRender::GzScaleMat(GzCoord scale, GzMatrix mat)
{
/* HW 3.5
// Create scaling matrix
// Pass back the matrix using mat value
*/
	GzMatrix scal = {
		scale[0], 0, 0, 0,
		0, scale[1], 0, 0,
		0, 0, scale[2], 0,
		0, 0, 0, 1
	};
	Replace(mat, scal);
	return GZ_SUCCESS;
}


GzRender::GzRender(int xRes, int yRes)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- set display resolution
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- allocate memory for pixel buffer
 */
	xres = xRes;
	yres = yRes;

	pixelbuffer = new GzPixel[xRes * yRes];
	framebuffer = (char*) malloc (3 * sizeof(char) * xRes * yRes);

/* HW 3.6
- setup Xsp and anything only done once 
- init default camera 
*/ 
	GzMatrix xsp = {
		xRes / 2, 0, 0, xRes / 2,
		0, -yRes / 2, 0, yRes / 2,
		0, 0, MAXINT, 0,
		0, 0, 0, 1
	};

	m_camera.FOV = DEFAULT_FOV;

	m_camera.position[0] = DEFAULT_IM_X;
	m_camera.position[1] = DEFAULT_IM_Y;
	m_camera.position[2] = DEFAULT_IM_Z;

	Replace(Xsp, xsp);
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
	for (int i = 0; i < xres * yres; ++i) {
		pixelbuffer[i].red = 200 << 4;
		pixelbuffer[i].blue = 200 << 4;
		pixelbuffer[i].green = 200 << 4;
		pixelbuffer[i].alpha = 1;
		pixelbuffer[i].z = MAXINT;
	}
	return GZ_SUCCESS;
}

int GzRender::GzBeginRender()
{
/* HW 3.7 
- setup for start of each frame - init frame buffer color,alpha,z
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms when needed 
*/ 
	for (int i = 0; i < xres * yres * 3; ++i) {
		framebuffer[i] = '0';
	}

	//Set Xpi and Xiw
	GzMatrix xpi = {
		1, 0, 0 ,0,
		0, 1, 0, 0,
		0, 0, tanf(ToRadian(m_camera.FOV / 2)), 0,
		0, 0, tanf(ToRadian(m_camera.FOV / 2)), 1
	};

	//Compute camera local Z axis
	GzCoord localZ;
	Subtract(m_camera.lookat, m_camera.position, localZ);
	Normalize(localZ);

	//Get up'
	GzCoord localY;
	Multiply(localZ, Dot(m_camera.worldup, localZ), localY);
	Subtract(m_camera.worldup, localY, localY);
	Normalize(localY);

	//Get Local X
	GzCoord localX;
	Cross(localY, localZ, localX);
	Normalize(localX);

	//Build Xiw
	GzMatrix xiw = {
		localX[X], localX[Y], localX[Z], -Dot(localX, m_camera.position),
		localY[X], localY[Y], localY[Z], -Dot(localY, m_camera.position),
		localZ[X], localZ[Y], localZ[Z], -Dot(localZ, m_camera.position),
		0, 0, 0, 1
	};
	Replace(m_camera.Xpi, xpi);
	Replace(m_camera.Xiw, xiw);


	matlevel = -1;
	GzPushMatrix(Xsp);
	GzPushMatrix(m_camera.Xpi);
	GzPushMatrix(m_camera.Xiw);
	return GZ_SUCCESS;
}

int GzRender::GzPutCamera(GzCamera camera)
{
/* HW 3.8 
/*- overwrite renderer camera structure with new camera definition
*/
	m_camera.FOV = camera.FOV;
	for (int i = 0; i < 3; ++i) {
		m_camera.lookat[i] = camera.lookat[i];
		m_camera.position[i] = camera.position[i];
		m_camera.worldup[i] = camera.worldup[i];
	}
	return GZ_SUCCESS;	
}

int GzRender::GzPushMatrix(GzMatrix	matrix)
{
/* HW 3.9 
- push a matrix onto the Ximage stack
- check for stack overflow
*/
	//char label[4][10] = { "Xsp", "Xpi", "Xiw", "Xwm" };
	//printf("Pushing %s\n", label[matlevel + 1]);
	
	GzMatrix identity = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	if (matlevel >= MATLEVELS) {
		printf("Stack Overflow\n");
		return GZ_FAILURE;
	}
	else {
		if (matlevel == -1) {
			Replace(Ximage[0], matrix);
			Replace(Xnorm[0], identity);
		}
		else {
			GzMatrix mat;
			Replace(mat, matlevel == 0 ? identity : matrix);
			Uniform(mat);
			Multiply(Ximage[matlevel], matrix, Ximage[matlevel + 1]);
			/*printf("Pushing Level: %d\n", matlevel + 1);
			Print(mat);
			PrintNew();*/
			Multiply(Xnorm[matlevel], mat, Xnorm[matlevel + 1]);
		}
		matlevel++;
	}
	return GZ_SUCCESS;
}

int GzRender::GzPopMatrix()
{
/* HW 3.10
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if (matlevel <= -1) {
		printf("Stack Underflow\n");
		return GZ_FAILURE;
	}
	else {
		matlevel--;
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


int GzRender::GzFlushDisplay2File(FILE* outfile)
{
/* HW1.6 write image to ppm file -- "P6 %d %d 255\r" */
	fprintf(outfile, "P6 %d %d 255\n", xres, yres);
	for (int i = 0; i < xres * yres; ++i) {
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
	for (int i = 0; i < xres * yres; ++i) {
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

	for (int i = 0; i < numAttributes; ++i) {
		switch (nameList[i]) {
			case GZ_RGB_COLOR:
				Replace(flatcolor, (float*)valueList[i]);
				break;
			case GZ_INTERPOLATE:
				interp_mode = *(int*)valueList[i];
				//printf("%d\n", interp_mode);
				break;
			case GZ_DIRECTIONAL_LIGHT:
				AddLight(this, (GzLight*)valueList[i]);
				break;
			case GZ_AMBIENT_LIGHT:
				Replace(&ambientlight, (GzLight*)valueList[i]);
				//printf("Ambient Light: \n");
				//Print(ambientlight);
				break;
			case GZ_AMBIENT_COEFFICIENT:
				Replace(Ka, (float*)valueList[i]);
				//printf("Ambient Coefficient: ");
				//Print(Ka);
				break;
			case GZ_DIFFUSE_COEFFICIENT:
				Replace(Kd, (float*)valueList[i]);
				//printf("Diffuse Coefficient: ");
				//Print(Kd);
				break;
			case GZ_SPECULAR_COEFFICIENT:
				Replace(Ks, (float*)valueList[i]);
				//printf("Specular Coefficient: ");
				//Print(Ks);
				break;
			case GZ_DISTRIBUTION_COEFFICIENT:
				spec = *(float*)valueList[i];
				//printf("Distribution Coefficient: %f\n", spec);
				break;
		}
	}
	
	return GZ_SUCCESS;
}



int GzRender::GzPutTriangle(int numParts, GzToken *nameList, GzPointer *valueList)
/* numParts - how many names and values */
{
/* HW 2.2
-- Pass in a triangle description with tokens and values corresponding to
      GZ_NULL_TOKEN:		do nothing - no values
      GZ_POSITION:		3 vert positions in model space
-- Invoke the rastrizer/scanline framework
-- Return error code
*/
	

	GzCoord* coordsT = NULL;
	GzCoord* coordsN = NULL;

	for (int i = 0; i < numParts; ++i) {
		switch (nameList[i]) {
			case GZ_POSITION:
				//2.2.1~2.2.2
				
				coordsT = static_cast<GzCoord*>(valueList[i]);
				//3.11 Transform vertex coordinates using current TOS matrix in Ximage stack
				if (TransformVertex(Ximage[matlevel], coordsT)) break;

				///////////////////////
				
				break;
			case GZ_NORMAL: 
				coordsN = static_cast<GzCoord*>(valueList[i]);
				TransformVertex(Xnorm[matlevel], coordsN);
				//Debug::Log(coordsN);
				break;
		}
		
	}
	
	ScanLine(this, coordsT, coordsN);
	return GZ_SUCCESS;
}

