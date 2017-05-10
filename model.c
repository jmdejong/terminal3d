
#include <stdlib.h>
#include <assert.h>
#include "model.h"


Shape new_Line(int p0, int p1){
	int* vertices = malloc(sizeof(int)*2); //{p0, p1};
	assert(vertices != NULL);
	vertices[0] = p0;
	vertices[1] = p1;
	Shape shape = {2, vertices, 0};
	return shape;
}

Shape new_Triangle(int p0, int p1, int p2){
	int* vertices = malloc(sizeof(int)*3); //{p0, p1, p2};
	assert(vertices != NULL);
	vertices[0] = p0;
	vertices[1] = p1;
	vertices[2] = p2;
	Shape shape = {3, vertices, 0};
	return shape;
}

void free_Shape(Shape shape){
	free(shape.vertices);
}


Model3d new_Model3d(){
	Model3d model = malloc(sizeof(struct Model3d));
	assert(model != NULL);
	model->vertexMax = 20;
	model->vertexLength = 0;
	model->vertices = malloc(sizeof(Vec3)*model->vertexMax);
	assert(model->vertices != NULL);
	model->shapeMax = 10;
	model->shapeLength = 0;
	model->shapes = malloc(sizeof(Shape)*model->shapeMax);
	assert(model->shapes != NULL);
	
	return model;
}

void free_Model3d(Model3d model){
	free(model->vertices);
	int i;
	for (i=0; i<model->shapeLength; i++){
		free_Shape(model->shapes[i]);
	}
	free(model->shapes);
	free(model);
}

int Model3d_addVertex(Model3d model, Vec3 vertex){
	int i = model->vertexLength;
	(model->vertexLength)++;
	if (model->vertexLength >= model->vertexMax){
		(model->vertexMax) *= 2;
		model->vertices = realloc(model->vertices, sizeof(Vec3) * model->vertexMax);
		assert(model->vertices != NULL);
	}
	model->vertices[i] = vertex;
	return i;
}

void Model3d_addShape(Model3d model, Shape shape){
	int i = model->shapeLength;
	(model->shapeLength)++;
	if (model->shapeLength >= model->shapeMax){
		(model->shapeMax) *= 2;
		model->shapes = realloc(model->shapes, sizeof(Shape) * model->shapeMax);
		assert(model->shapes != NULL);
	}
	model->shapes[i] = shape;
// 	printf("adding a line: %d, %d\n", shape.vertices[0], shape.vertices[1]);
}

void Model3d_addLine(Model3d model, Vec3 vertex0, Vec3 vertex1){
	int p0 = Model3d_addVertex(model, vertex0),
		p1 = Model3d_addVertex(model, vertex1);
	
	Model3d_addShape(model, new_Line(p0, p1));
}

