/*
 * testHarris.c
 *
 *  Created on: Nov 14, 2015
 *      Author: ra146446
 */

#include "iftHarris.h"

int main(int argc, char *argv[]) {

	if (argc < 2) {
		printf("usage:\n\t %s <image_path>\n", argv[0]);
		exit(1);
	}

	iftImage *img = iftReadImageByExt(argv[1]);

	iftImage *corners;
	iftVoxel conerList[img->n];
	int len;

	iftHarris(img, 10000, &corners, conerList, &len);

	iftWriteImageP2(corners, "corners.pgm");

	iftDestroyImage(&img);
	iftDestroyImage(&corners);



	return 0;
}
