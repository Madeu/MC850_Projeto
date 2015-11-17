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
void iftHarris(iftImage *img, float threshold, iftImage **corners, iftVoxel *cornersList, int *len);

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

void iftHarris(iftImage *img, float threshold, iftImage **corners, iftVoxel *cornersList, int *len) {
	iftImage *gx;
	iftImage *gy;

	iftImage *aux[3];
	iftImage *ix2;
	iftImage *iy2;
	iftImage *ixy;

	iftKernel *gaussianKrnl = iftGaussianKernel2D(3, 4.0);

	int p, q, i;
	float r;
	float gx2, gy2, gxy;
	float det, trace;
	float k = 0.04;

	iftAdjRel* adj = iftRectangular(9, 3);
	iftVoxel v;
	iftVoxel u;
	int current;
	int voxelId = 0;

	gradient(img, &gx, &gy);

	aux[0] = iftMult(gx, gx);
	aux[1] = iftMult(gy, gy);
	aux[2] = iftMult(gx, gy);

	ix2 = iftFastLinearFilter(aux[0], gaussianKrnl, 0);
	iy2 = iftFastLinearFilter(aux[1], gaussianKrnl, 0);
	ixy = iftFastLinearFilter(aux[2], gaussianKrnl, 0);

	*corners = iftCreateImage(img->xsize, img->ysize, img->zsize);

	/** R = Det(H) - k (Trace(H))^2 **/

	for (p = 0; p < img->n; ++p) {

		det = ix2->val[p] * iy2->val[p] - ixy->val[p] * ixy->val[p];
		trace = ix2->val[p] + iy2->val[p];

		r = det - k * pow(trace, 2.0);


		if (r > threshold)
			(*corners)->val[p] = r; // for visualization purposes
		else
			(*corners)->val[p] = 0;
	}

	/** Nonmaximal supression **/

	for (p = 0; p < (*corners)->n; ++p) {
		current = (*corners)->val[p];
		iftVoxel v = iftGetVoxelCoord((*corners), p);

		for (i = 0; i < adj->n; ++i) {
			iftVoxel u = iftGetAdjacentVoxel(adj, v, i);
			if (iftValidVoxel(img, u)) {
				q = iftGetVoxelIndex((*corners), u);
				if ((*corners)->val[q] > current) {
					current = 0;
					break;
				}
			}
		}

		if (current != 0)
			cornersList[voxelId++] = v;
	}
	*len = voxelId;

	iftDestroyImage(&gx);
	iftDestroyImage(&gy);
	iftDestroyImage(&ix2);
	iftDestroyImage(&iy2);
	iftDestroyImage(&ixy);
	iftDestroyImage(&aux[0]);
	iftDestroyImage(&aux[1]);
	iftDestroyImage(&aux[2]);
	iftDestroyKernel(&gaussianKrnl);

}

#endif /* IFTHARRIS_H_ */
