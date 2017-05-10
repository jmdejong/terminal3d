

#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "drawfield.h"

#define max(x,y) (((x)>(y))?(x):(y))

Drawfield new_Drawfield(int width, int height){
	
	Drawfield df = malloc(sizeof(struct Drawfield));
	assert(df != NULL);
	
	df->width = width;
	df->height = height;
	df->originX = 0;
	df->originY = 0;
	df->signs = malloc(sizeof(char)*width*height);
	assert(df->signs != NULL);
	df->depths = malloc(sizeof(float)*width*height);
	assert(df->depths != NULL);
	
	return df;
}

void free_Drawfield(Drawfield df){
	
	free(df->signs);
	free(df->depths);
	free(df);
	
}

void Drawfield_clear(Drawfield df){
	
	int i;
	for (i=0; i<df->width*df->height; i++){
		df->signs[i] = ' ';
		df->depths[i] = 0.0;
	}
}


void Drawfield_setOrigin(Drawfield df, int x, int y){
	df->originX = x;
	df->originY = y;
}

void Drawfield_set(Drawfield df, char sign, int x, int y, float z){
	x += df->originX;
	y += df->originY;
	if (x < 0 || y < 0 || x >= df->width || y >= df->height){
		return;
	}
	int id = x + y * df-> width;
	if (z > 0 && (df->depths[id] <= 0 || z <= df->depths[id])){
		df->signs[id] = sign;
		df->depths[id] = z;
	}
}

char Drawfield_get(Drawfield df, int x, int y){
	x += df->originX;
	y += df->originY;
	if (x < 0 || y < 0 || x >= df->width || y >= df->height){
		return '\0';
	}
	return df->signs[x + y * df->width];
}

void Drawfield_print(Drawfield df){
	int x, y,
		w = df->width,
		h = df->height,
		xmin = -df->originX,
		ymin = -df->originY,
		xmax = w - df->originX,
		ymax = h - df->originY;
	printf(" --");
	for (x=0; x<w; x++)
		printf("--");
// 	for (y=ymax-1; y>=ymin; y--){
	for (y=ymin; y<ymax; y++){
		printf("\n |");
		for (x=xmin; x<xmax; x++){
			if (Drawfield_get(df,x,y))
				printf("%c ",Drawfield_get(df,x,y));
			else
				printf("  ");
		}
		printf("|");
	}
	printf("\n --");
	for (x=0;x<w;x++){
		printf("--");
	}
	printf("\n");
}


void Drawfield_line3d(Drawfield df, char sign, Vec3 p0, Vec3 p1){
	
	int xmin = -df->originX,
		ymin = -df->originY,
		xmax = df->width - df->originX,
		ymax = df->height - df->originY;
	
	double dx = p1.x-p0.x, dy = p1.y-p0.y, dz = p1.z-p0.z;
	double length = sqrt(dx*dx+dy*dy);
	/*normalize(&dx,&dy);*/
	if (length){
		dx/=length;
		dy/=length;
		dz/=length;
	} else {
		if (!sign){
			sign = '-';
		}
		Drawfield_set(df, sign , (int)p0.x, (int)p0.y, p0.z);
		return;
	}
	
	if (!sign){
		char* lineshapes = "*-/|\\";
		sign = lineshapes[abs((int)(dy*1.3+0.5-(dy<0))*3 + (int)(dx*1.3+0.5-(dx<0)))];
// 		printf("dx: %f, dy: %f, sign: %c\n", dx, dy, sign);
	}
	double x=p0.x, y=p0.y, z=p0.z;
	
	// code I wrote earlier: ugly, but it works
	for (x=p0.x, y=p0.y; ((dy>0)?(y<=p1.y&&y<ymax):(y>=p1.y&&y>=ymin))&&((dx>0)?(x<=p1.x&&x<xmax):(x>=p1.x&&x>=xmin)); x+=dx, y+=dy, z+=dz){
		Drawfield_set(df, sign, (int)x, (int)y, z);
	}
	
}

void Drawfield_triangle3d(Drawfield df, char sign, Vec3 p0, Vec3 p1, Vec3 p2){
	
	int xmin = -df->originX,
		xmax = df->width - df->originX;
	
	if (!sign){
		sign = '*';
	}
	
	/* order points by x value */
	Vec3 temp;
	if (p1.x < p0.x){
		temp = p0;
		p0 = p1;
		p1 = temp;
	}
	if (p2.x < p0.x){
		temp = p0;
		p0 = p2;
		p2 = temp;
	}
	if (p2.x < p1.x){
		temp = p1;
		p1 = p2;
		p2 = temp;
	}
	
	// in this case it happens to be a straight vertical line
	// very unlikely, fill in later
	if (p2.x == p0.x){
		
		return;
	}
	double dy02=(p2.y-p0.y)/(p2.x-p0.x), dz02=(p2.z-p0.z)/(p2.x-p0.x);
	double x, dy, dz;
	if (p0.x != p1.x){
		dy = (p1.y-p0.y)/(p1.x-p0.x), dz = (p1.x-p0.x)/(p1.x-p0.x);
		for (x=max(0, xmin - p0.x); p0.x+x<=p1.x && p0.x+x<=xmax; x++){
			Drawfield_line3d(df, sign, new_Vec3(p0.x+x, p0.y+dy*x, p0.z+dz*x), new_Vec3(p0.x+x, p0.y+x*dy02, p0.z+x*dz02));
			//x0+dx02*(y-y0),y,z0+dz02*(y-y0),x0+dx01*(y-y0),y,z0+dz01*(y-y0));
		}
	}
	if (p1.x != p2.x){
		dy = (p2.y-p1.y)/(p2.x-p1.x), dz = (p2.x-p1.x)/(p2.x-p1.x);
		for (x=max(0, xmin - p1.x); p1.x+x<=p2.x && p1.x+x<=xmax; x++){
			Drawfield_line3d(df, sign, new_Vec3(p1.x+x, p1.y+dy*x, p1.z+dz*x), new_Vec3(p1.x+x, p0.y+x*dy02, p0.z+x*dz02));
		}
	}
	
}
