
#include <stdlib.h> // malloc, realloc, free
#include <assert.h> // assert
#include <curses.h> //
#include <string.h> // memcpy
#include <math.h>   // sin, cos
#include "vector3.h"
#include "drawfield.h"
#include "model.h"

#define USING_CURSES 1

/* I use underscore in names for simulating the new keyword and for functions
 * that would have been methods in other languages.
 * 
 * Also, unlike most C programmers, I prefer to put the * for declaring a
 * pointer directly after the type instead of directly before the name.
 */





typedef struct {
	Vec3 location;
	Vec3 orientation;
} Entity3d;


void moveCamera(Entity3d* camera, Vec3 location){
	
	camera->location.x += location.x * cos(camera->orientation.y) + location.z * sin(camera->orientation.y);
	camera->location.y += location.y;
	camera->location.z += location.z * cos(camera->orientation.y) - location.x * sin(camera->orientation.y);
}

void rotateCamera(Entity3d* camera, Vec3 orientation){
	
	camera->orientation.x += orientation.x;
	camera->orientation.y += orientation.y;
	camera->orientation.z += orientation.z;
}



Vec3* transformToView(const Vec3* vertices, int vertexLength, Entity3d camera){
	
	
	Vec3* newVertices = malloc(sizeof(Vec3) * vertexLength);
	assert(newVertices != NULL);
	
	// minimize the use of sin and cos
	double sinX = sin(camera.orientation.x), sinY = sin(camera.orientation.y), sinZ = sin(camera.orientation.z),
		cosX = cos(camera.orientation.x), cosY = cos(camera.orientation.y), cosZ = cos(camera.orientation.z);
	
	double x0 =  cosY * cosZ;
	double x1 =  cosY * sinZ;
	double x2 = -sinY;

	double y0 =  sinX * sinY * cosZ - cosX * sinZ;
	double y1 =  sinX * sinY * sinZ + cosX * cosZ;
	double y2 =  sinX * cosY;

	double z0 =  cosX * sinY * cosZ - sinX * sinZ;
	double z1 =  cosX * sinY * sinZ - sinX * cosZ;
	double z2 =  cosX * cosY;
	
	
	int i;
	for (i=0; i<vertexLength; i++){
		
		Vec3 vertex = new_Vec3(vertices[i].x - camera.location.x, vertices[i].y - camera.location.y, vertices[i].z - camera.location.z);
		
		newVertices[i] = new_Vec3(
			vertex.x * x0 + vertex.y * x1 + vertex.z * x2,
			vertex.x * y0 + vertex.y * y1 + vertex.z * y2,
			vertex.x * z0 + vertex.y * z1 + vertex.z * z2
		);
	}
	
	return newVertices;
	
}

void setPerspective(Vec3* vertices, int length, double perspective){
	int i;
	for (i=0; i<length; i++){
		double z=abs(vertices[i].z);
		if (z==0.0){
			z=0.0001;
		}
// 		float dist = sqrt(x*x,y*y,z*z);
		vertices[i].x *= perspective/z;
		vertices[i].y *= perspective/z;
// 		vertices[i].z = dist;
	}
}



void draw3d(Model3d model, Entity3d camera, Drawfield df){
	
	
	
	Vec3* drawVertices = transformToView(model->vertices, model->vertexLength, camera);
	setPerspective(drawVertices, model->vertexLength, 15);
	
	int i;
	for (i=0; i<model->shapeLength; i++){
		Shape shape = model->shapes[i];
		
		if (shape.length == 2){
// 			printf("vertex indices: size: %d, vertex0: %d vertex1: %d of: %d\n", shape.length, shape.vertices[0], shape.vertices[1], model->vertexLength);
			Drawfield_line3d(df, shape.sign, drawVertices[shape.vertices[0]], drawVertices[shape.vertices[1]]);
// 			printf("line: %f,%f,%f  %f,%f,%f\n", drawVertices[shape.vertices[0]].x, drawVertices[shape.vertices[0]].y, drawVertices[shape.vertices[0]].z, drawVertices[shape.vertices[1]].x, drawVertices[shape.vertices[1]].y, drawVertices[shape.vertices[1]].z);
		} else if (shape.length == 3){
			Drawfield_triangle3d(df, shape.sign, drawVertices[shape.vertices[0]], drawVertices[shape.vertices[1]], drawVertices[shape.vertices[2]]);
		} else {
			printf("Only lines and triangles are supported so far");
			exit(-1);
		}
	}
	
	free(drawVertices);
}


#if USING_CURSES

void printDrawfieldCurses(Drawfield df){
	int originX = df->originX, originY = df->originY;
	Drawfield_setOrigin(df, 0, 0);
	int x, y;
	for (x=0; x<df->width; x++){
		for(y=0; y<df->height; y++){
			mvaddch(y, x*2, Drawfield_get(df, x, y));
		}
	}
	refresh();
	Drawfield_setOrigin(df, originX, originY);
}

#endif

void update(Entity3d* camera, Model3d model, Drawfield df, char input ){
	
	
	
	
	switch (input){
		case 'w':
			moveCamera(camera, new_Vec3(0,0,1));
			break;
		case 's':
			moveCamera(camera, new_Vec3(0,0,-1));
			break;
		case 'e':
			moveCamera(camera, new_Vec3(1,0,0));
			break;
		case 'q':
			moveCamera(camera, new_Vec3(-1,0,0));
			break;
		case ' ':
			moveCamera(camera, new_Vec3(0,1,0));
			break;
		case 'z':
			moveCamera(camera, new_Vec3(0,-1,0));
			break;
		case 'a':
			rotateCamera(camera, new_Vec3(0,-0.1,0));
			break;
		case 'd':
			rotateCamera(camera, new_Vec3(0,0.1,0));
			break;
		case 'f':
			rotateCamera(camera, new_Vec3(-0.1,0,0));
			break;
		case 'g':
			rotateCamera(camera, new_Vec3(0.1,0,0));
			break;
		case 'h':
			rotateCamera(camera, new_Vec3(0,0,-0.1));
			break;
		case 'j':
			rotateCamera(camera, new_Vec3(0,0,0.1));
			break;
	}
	
	Drawfield_clear(df);
	
	draw3d(model, *camera, df);
	
}


int main(int argc, char** argv){
	
	
	Entity3d camera = {new_Vec3(0,0,0), new_Vec3(0,0,0)};
	
	Model3d model = new_Model3d();
	
	
	// first letter: y coordinate: top/bottom. second: x: left/right. third: z: front/back.
	int tlf = Model3d_addVertex(model, new_Vec3(-5,5,10)),
		trf = Model3d_addVertex(model, new_Vec3(5,5,10)),
		blf = Model3d_addVertex(model, new_Vec3(-5,-5,10)),
		brf = Model3d_addVertex(model, new_Vec3(5,-5,10)),
		tlb = Model3d_addVertex(model, new_Vec3(-5,5,20)),
		trb = Model3d_addVertex(model, new_Vec3(5,5,20)),
		blb = Model3d_addVertex(model, new_Vec3(-5,-5,20)),
		brb = Model3d_addVertex(model, new_Vec3(5,-5,20));
		
	
	Model3d_addShape(model, new_Triangle(trf, trb, brf));
	Model3d_addShape(model, new_Triangle(brb, trb, brf));
	
	Model3d_addShape(model, new_Line(tlf, trf));
	Model3d_addShape(model, new_Line(blf, brf));
	Model3d_addShape(model, new_Line(tlb, trb));
	Model3d_addShape(model, new_Line(blb, brb));
	Model3d_addShape(model, new_Line(tlf, blf));
	Model3d_addShape(model, new_Line(trf, brf));
	Model3d_addShape(model, new_Line(tlb, blb));
	Model3d_addShape(model, new_Line(trb, brb));
	Model3d_addShape(model, new_Line(tlf, tlb));
	Model3d_addShape(model, new_Line(trf, trb));
	Model3d_addShape(model, new_Line(blf, blb));
	Model3d_addShape(model, new_Line(brf, brb));
	
	
	int w = 60, h = 30;
#if USING_CURSES
	initscr();			/* Start curses mode 		  */
	cbreak();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(FALSE);
	// yes, w and h are ints, not pointers: getxymax is a macro
	getmaxyx(stdscr, h, w);
#endif
	Drawfield df = new_Drawfield(w/2, h);
	Drawfield_setOrigin(df, df->width/2, df->height/2);
	
	char input = 0;
	while (input != '0'){
		update(&camera, model, df, input);
#if USING_CURSES
		printDrawfieldCurses(df);
		input = getch();
#else
		Drawfield_print(df);
		input = getchar();
#endif
	}
	
#if USING_CURSES
	endwin();
#endif
	
	free_Model3d(model);
	free_Drawfield(df);
	
// 	_nc_free_and_exit(0);
	
	return 0;
}