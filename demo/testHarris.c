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

	iftHarris(img, &corners, 10);

	iftWriteImageP2(corners, "corners.pgm");

	iftDestroyImage(&img);
	iftDestroyImage(&corners);



	return 0;
}
