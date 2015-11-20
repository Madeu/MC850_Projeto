/*
 * testHarris.c
 *
 *  Created on: Nov 14, 2015
 *      Author: ra146446
 */

#include "iftHarris.h"
#include "iftSeedsGenerator.h"

int main(int argc, char *argv[]) {

	if (argc < 3) {
		printf("usage:\n\t %s <image_path> <num_centroids>\n", argv[0]);
		exit(1);
	}

	iftImage *img = iftReadImageByExt(argv[1]);
	iftImage *imgc = iftReadImageByExt(argv[1]);

	iftDataSet* dataset;
	iftImage *corners;
	iftVoxel conerList[img->n];
	int len;

	iftHarris(img, 1000, &corners, conerList, &len);
	plotPoint(img, conerList, len);
//	iftVoxel* centersPos = generateCentroids(&dataset, conerList, len, atoi(argv[2]));

	iftWriteImageP2(corners, "corners.pgm");
	iftWriteImageP2(img, "img-corners.pgm");

//	plotPoint(imgc, centersPos, atoi(argv[2]));
//	iftWriteImageP2(imgc, "img-centers.pgm");

//	int *counter = countGroup(dataset);
//	char buf[20];
//
//	for (int i = 0; i < atoi(argv[2]); i++ ){
//			printf("%d - [%d, %d]\n", i, centersPos[i].x, centersPos[i].y);
//	}
//
//	for (int i = 0; i < atoi(argv[2]); i++) {
//		printf("[%d] - > %d\n", i, counter[i]);
//		sprintf(buf, "group-%d.pgm", i);
//		iftImage* imgaux = iftReadImageByExt(argv[1]);
//		iftVoxel* voxels = (iftVoxel*) malloc(counter[i]*sizeof(iftVoxel));
//		int k = 0;
//
//		for (int j = 0; j < dataset->nsamples; j++) {
//			if(dataset->sample[j].label == (i+1)){
//				voxels[k].x = (int)dataset->sample[j].feat[0];
//				voxels[k].y = (int)dataset->sample[j].feat[1];
//				voxels[k].z = 0;
//				k++;
//			}
//		}
//
//		plotPoint(imgaux, voxels, counter[i]);
//
//		free(voxels);
//		iftWriteImageP2(imgaux, buf);
//		iftDestroyImage(&imgaux);
//	}

	iftDestroyImage(&img);
	iftDestroyImage(&imgc);
	iftDestroyImage(&corners);
//	free(centersPos);
//	free(counter);

	return 0;
}
