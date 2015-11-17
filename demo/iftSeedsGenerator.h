#include "ift.h"
#include <string.h>

#define EPSILON 0.0001
#define MAX_INTERATION 1000

/**
	Function that read a file which contians points in the format:
		|number of points
		|point1X point1Y
		|point2X point2Y
		|point3X point3Y
		...
	params:
	-filename: string of the file name
	-u : array of iftVoxels to write the points.

	return:
	-size of u
*/

int readPoint(char *filename, iftVoxel* u) {
	FILE *fp;
	char ch, buffer[64], *pch;
	int x, y, size, i = 0, voxel_id = 0;

	fp = fopen(filename,"r"); // read mode
 
	if(fp == NULL) {
		perror("Error while opening the file.\n");
		return -1;
	}

	while( ( ch = fgetc(fp) ) != '\n' ) {
		buffer[i] = ch;
		i++;
	}
	buffer[i] = '\0';
	size = atoi(buffer);
	
	i = 0;
	while( ( ch = fgetc(fp) ) != EOF ) {
		if (ch == '\n' || ch == ' ') {
			buffer[i] = '\0';
			i = 0;
			if (ch == ' '){
				u[voxel_id].x = atoi(buffer);
			}
			else {
				u[voxel_id].y = atoi(buffer);
				u[voxel_id].z = 0;
				voxel_id++;
			}
		}
		else {
			buffer[i] = ch;
			i++;
		}
	}

	u[voxel_id].y = atoi(buffer);
	u[voxel_id].z = 0;
	voxel_id++;

	fclose(fp);

	return size;
}

/**
	Plot points on the image.
	params:
	-img : img target
	-u : array with point to mark on image
	-len : size of u (number of points)
*/
void plotPoint(iftImage* img, iftVoxel* u, int len) {
	iftAdjRel *A = iftCircular(2.0);

	iftColor color;
	color.val[0] = 1.0;
	color.val[1] = 1.0;
	color.val[2] = 1.0;

	iftColor pcolor = iftRGBtoYCbCr(color, 0);

	for (int i = 0; i < len; i++) {
		iftDrawPoint(img, u[i], pcolor, A);
	}

    iftDestroyAdjRel(&A);
}

/**
	Function to create a data set base on points im 2D plane,
	which will be an input to Cluster algorithm.
*/
iftDataSet* createDataSet(iftVoxel* points, int size){

	iftDataSet* dataSet = iftCreateDataSet(size, 2);
	iftSample* samples = (iftSample *) malloc(size*sizeof(iftSample));

	for (int i = 0; i < size; i++) {
		float* feature = (float *) malloc(2*sizeof(float));
		
		feature[0] = points[i].x;
		feature[1] = points[i].y;

		samples[i].feat = feature;
		samples[i].id = i+1;
	}

	dataSet->sample = samples;
	return dataSet;
}

iftDataSet* generateCentroids(iftVoxel* points, int size) {
	iftDataSet* dataSet = createDataSet(points, size);
	iftDataSet* centers = iftKmeansInitCentroidsFromSamples(dataSet, 2);
	iftKmeansRun(0, dataSet, &centers, MAX_INTERATION, EPSILON);

	iftDestroyDataSet(&dataSet);

	return centers;
}