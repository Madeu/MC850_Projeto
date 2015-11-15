/*
 * iftHarris.h
 *
 *  Created on: Nov 14, 2015
 *      Author: ra146446
 */

#ifndef IFTHARRIS_H_
#define IFTHARRIS_H_

#include "ift.h"

/**
 * Calculates image gradient.
 *
 * @param[in]	img			gray scale image.
 * @param[out]	grad		gradient magnitude.
 * @param[out]	gradX		gradient magnitude in x.
 * @param[out]	gradY		gradient magnitude in y.
 */
void gradient(iftImage *img, iftImage **gradX, iftImage **gradY);

/**
 * Calculates Harris Corners detector.
 *
 * @param[in]	img			gray scale image.
 * @param[out]	corners		corners detected.
 */
void iftHarris(iftImage *img, iftImage **corners, float threshold);

/**================================================================================**/
/**================================================================================**/

void gradient(iftImage *img, iftImage **gradX, iftImage **gradY) {
	iftVoxel v;
	iftVoxel u;
	float r = 5.0;
	float sigma = r / 3;
	iftAdjRel* adj = iftCircular(r);

	int p;
	int q;
	int i;

	float g;
	float gx;
	float gy;
	float factor;
	float distance;
	float xPart;
	float yPart;
	float _const = 180 / PI;

	*gradX = iftCreateImage(img->xsize, img->ysize, img->zsize);
	*gradY = iftCreateImage(img->xsize, img->ysize, img->zsize);

	//foreach pixel in the image
	for (p = 0; p < img->n; ++p) {
		//gets the multidimensional coordinate using the unidimensional index
		v = iftGetVoxelCoord(img, p);

		gx = 0;
		gy = 0;
		//foreach neighbor voxel in the adjacency
		for (i = 1; i < adj->n; ++i) {

			u = iftGetAdjacentVoxel(adj, v, i);

			//check if it is a valid voxel (is it inside the image?)
			if (iftValidVoxel(img, u)) {
				q = iftGetVoxelIndex(img, u);

				distance = sqrt(pow(v.x - u.x, 2.0) + pow(v.y - u.y, 2.0));
				factor = (img->val[p] - img->val[q])
						* exp(-(pow(distance, 2.0) / (2 * pow(sigma, 2.0))));

				xPart = (u.x - v.x) / distance;
				yPart = (u.y - v.y) / distance;

				gx += factor * xPart;
				gy += factor * yPart;

				(*gradX)->val[p] = gx;
				(*gradY)->val[p] = gy;


			}
		}

	}
	iftDestroyAdjRel(&adj);
}

/**================================================================================**/

void iftHarris(iftImage *img, iftImage **corners, float threshold) {
	iftImage *gx;
	iftImage *gy;

	int p;
	float r;
	float gx2, gy2, gxy;
	float det, trace;
	float k = 0.04;

	gradient(img, &gx, &gy);

	/**TODO Smooth gradients? **/

	*corners = iftCreateImage(img->xsize, img->ysize, img->zsize);

	/** R = Det(H) - k (Trace(H))^2 **/

	for (p = 0; p < img->n; ++p) {
		gx2 = pow(gx->val[p], 2.0);
		gy2 = pow(gy->val[p], 2.0);
		gxy = gy->val[p] * gx->val[p];

		det = gx2 * gy2 - gxy * gxy;
		trace = gx2 + gy2;

		r = det - k * pow(trace, 2.0);

		if (r > threshold)
			(*corners)->val[p] = 255; // for visualization purposes
		else
			(*corners)->val[p] = 0;

		printf("gx2: %.1f\t", gx2);
		printf("gy2: %.1f\t", gy2);
		printf("gxy: %.1f\t", gxy);
		printf("det: %.1f\t", det);
		printf("trace: %.1f\t", trace);
		printf("r: %.1f\n", r);
	}


}



#endif /* IFTHARRIS_H_ */
