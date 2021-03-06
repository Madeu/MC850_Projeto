#include "ift.h"
#include "iftHarris.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define EPSILON 0.00001
#define MAX_INTERATION 100000

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
	iftAdjRel *A = iftCircular(2.5);

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

	iftDataSet* dataset = iftCreateDataSet(size, 2);
	iftSample* samples = (iftSample *) malloc(size*sizeof(iftSample));

	for (int i = 0; i < size; i++) {
		float* feature = (float *) malloc(2*sizeof(float));
		
		feature[0] = points[i].x;
		feature[1] = points[i].y;

		samples[i].feat = feature;
		samples[i].id = i+1;
		samples[i].label = 0;
	}

	dataset->sample = samples;
	return dataset;
}

int* countGroup(iftDataSet* dataset) {
	int num_centers = dataset->nfeats;
	int* count = malloc(num_centers*sizeof(int));

	for(int i = 0; i < num_centers; i++) {
		count[i] = 0;
	}

	for(int i = 0; i < dataset->nsamples; i++) {
		count[dataset->sample[i].label - 1] += 1;
	}

	return count;
}

iftVoxel* generateCentroids(iftDataSet** dataset, iftVoxel* points, int size, int num_centers) {
	*dataset = createDataSet(points, size);

	iftDataSet* centers = iftKmeansInitCentroidsFromSamples((*dataset), num_centers);
	iftKmeansRun(0, (*dataset), &centers, MAX_INTERATION, EPSILON);
	
	iftVoxel *centersPos = (iftVoxel*) malloc(centers->nsamples*sizeof(iftVoxel));
	for(int i = 0; i < centers->nsamples; i++) {
		centersPos[i].x = (int)centers->sample[i].feat[0];
		centersPos[i].y = (int)centers->sample[i].feat[1];
		centersPos[i].z = 0;
	}

	iftDestroyDataSet(&centers);

	return centersPos;
}

iftVoxel getBigCenter(iftImage* img ,int harris_number, int num_centers){
	int len, max, maxpos = 0;
	iftDataSet* dataset;
	iftImage *corners;
	iftVoxel corner_list[img->n];

	iftHarris(img, harris_number, &corners, corner_list, &len);
	iftVoxel* centers_pos = generateCentroids(&dataset, corner_list, len, num_centers);

	int *counter = countGroup(dataset);

	max = counter[0];
	for (int i = 0; i < num_centers; i++) {
		if(max < counter[i]) {
			max = counter[i];
			maxpos = i;
		}
	}

	iftVoxel voxel;
	voxel.x = centers_pos[maxpos].x;
	voxel.y = centers_pos[maxpos].y;
	voxel.z = centers_pos[maxpos].z;

	iftDestroyImage(&corners);
	iftDestroyDataSet(&dataset);
	free(counter);
	free(centers_pos);

	return voxel;
}

iftImage *drawLabelGen(iftImage *img, iftVoxel center, int delta_x, int delta_y) {
	iftImage *marker = iftCreateImage(img->xsize, img->ysize, 1);

	iftVoxel aux;

	int x, y;	
	for(x = -delta_x; x < delta_x; x++){
		for(y = -delta_y; y < delta_y; y++){
			aux.x = center.x+x;
			aux.y = center.y+y;
			aux.z = center.z;

			if(iftValidVoxel(img, aux)) {
				int coord = iftGetVoxelIndex(marker, aux);
		        marker->val[coord] = 255;
			}
	
		}
	}

	return marker;
}