

#ifndef MODEL3D
#define MODEL3D

#include "vector3.h"


// actually just an array, or maybe more like a tuple
typedef struct Shape {
	int length;
	int* vertices;
	char sign;
} Shape;

Shape new_Line(int p0, int p1);

Shape new_Triangle(int p0, int p1, int p2);


typedef struct Model3d {
	Vec3* vertices;
	int vertexMax;
	int vertexLength;
	Shape* shapes;
	int shapeMax;
	int shapeLength;
}* Model3d;


Model3d new_Model3d();

void free_Model3d(Model3d model);

int Model3d_addVertex(Model3d model, Vec3 vertex);

void Model3d_addShape(Model3d model, Shape shape);

void Model3d_addLine(Model3d model, Vec3 vertex0, Vec3 vertex1);

#endif