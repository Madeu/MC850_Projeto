/*
 * hog2.h
 *
 *  Created on: Nov 25, 2015
 *      Author: ra146446
 */

#ifndef HOG2_H_
#define HOG2_H_

#include "ift.h"
#include "hog_char.h"

iftFeatures *hog2(iftImage *img) {

	iftImage *img_normalized = normalize(img);
	iftImage *img_mag;
	iftImage *img_orient;
	gradient(img_normalized, &img_mag, &img_orient);
	iftDestroyImage(&img_normalized);

	return hog(img_mag, img_orient, img->xsize / 2, img->ysize / 2);
}

#endif /* HOG2_H_ */
