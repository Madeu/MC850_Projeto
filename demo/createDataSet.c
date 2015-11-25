/*
 * createDataSet.c
 *
 *  Created on: Nov 25, 2015
 *      Author: ra146446
 */

#include "ift.h"
#include "hog2.h"

int main(int argc, char *argv[]) {

	if (argc < 3) {
		printf("usage:\n");
		printf("\t%s <true-dir> <true-dir>\n", argv[0]);
		exit(1);
	}

	iftDir *trueDir = iftLoadFilesFromDirectory(argv[1], "pgm");
	iftDir *falseDir = iftLoadFilesFromDirectory(argv[2], "pgm");

	int descriptorSize = (HOG_N1 / HOG_N2 - HOG_N3 + 1)
			* (HOG_M1 / HOG_M2 - HOG_M3 + 1) * HOG_N3 * HOG_M3 * 9;
	int nOfSamples = trueDir->nfiles + falseDir->nfiles;
	iftDataSet *dataset = iftCreateDataSet(nOfSamples, descriptorSize);
	dataset->nclasses = 2;

	/* Extract true plates features and put in dataset*/
	iftImage *img;
	iftFeatures *feat;
	int candIndex = 0;

	for (int i = 0; i < trueDir->nfiles; ++i) {
		/* Get image from dir */
		img = iftReadImageByExt(trueDir->files[i]->pathname);
		/* Extract HoG */
		feat = hog2(img);

		/* Fill dataset */
		for (int f = 0; f < feat->n; f++) {
			dataset->sample[candIndex].feat[f] = feat->val[f];
		}
		dataset->sample[candIndex++].truelabel = 1;

		iftDestroyFeatures(&feat);
		iftDestroyImage(&img);
	}

	for (int i = 0; i < falseDir->nfiles; ++i) {
		/* Get image from dir */
		img = iftReadImageByExt(falseDir->files[i]->pathname);
		/* Extract HoG */
		feat = hog2(img);

		/* Fill dataset */
		for (int f = 0; f < feat->n; f++) {
			dataset->sample[candIndex].feat[f] = feat->val[f];
		}
		dataset->sample[candIndex++].truelabel = 2;

		iftDestroyFeatures(&feat);
		iftDestroyImage(&img);
	}

	/* Write dataset to file. */
	iftWriteOPFDataSet(dataset, "generated-dataset");

	printf("Dataset generated in file: generated-dataset.\n");

	/* Free memory*/
	iftDestroyDataSet(&dataset);
	iftDestroyDir(&trueDir);
	iftDestroyDir(&falseDir);

	return 0;
}
