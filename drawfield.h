
#ifndef DRAWFIELD
#define DRAWFIELD

#include "vector3.h"

typedef struct Drawfield {
	char* signs;
	float* depths;
	int width;
	int height;
	int originX;
	int originY;
}* Drawfield;


Drawfield new_Drawfield(int width, int height);

void free_Drawfield(Drawfield df);
void Drawfield_clear(Drawfield df);
void Drawfield_setOrigin(Drawfield df, int x, int y);
void Drawfield_set(Drawfield df, char sign, int x, int y, float z);
char Drawfield_get(Drawfield df, int x, int y);
void Drawfield_line3d(Drawfield df, char sign, Vec3 p0, Vec3 p1);
void Drawfield_triangle3d(Drawfield df, char sign, Vec3 p0, Vec3 p1, Vec3 p2);
void Drawfield_print(Drawfield field);


#endif