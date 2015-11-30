/*
 * createDataSet.c
 *
 *  Created on: Nov 25, 2015
 *      Author: ra146446
 */

#include "ift.h"
#include "hog_char2.h"

int main(int argc, char *argv[]) {

	if (argc < 2) {
		printf("usage:\n");
		printf("\t%s <true-dir> <false-dir>\n", argv[0]);
		exit(1);
	}

	printf("Using Hog\n");

	iftDir *trueDir = iftLoadFilesFromDirectory(argv[1], "pgm");
	//iftDir *falseDir = iftLoadFilesFromDirectory(argv[2], "pgm");

	int descriptorSize = (HOG_N1 / HOG_N2 - HOG_N3 + 1)
			* (HOG_M1 / HOG_M2 - HOG_M3 + 1) * HOG_N3 * HOG_M3 * 9;
	int nOfSamples = trueDir->nfiles;
	iftDataSet *dataset = iftCreateDataSet(nOfSamples, descriptorSize);
	dataset->nclasses = 10;

	/* Extract true plates features and put in dataset*/
	iftImage *img;
	iftFeatures *feat;
	int candIndex = 0;

	FILE *fp = fopen("train_dataset.dat", "w");

	int label = 1;

	fprintf(fp, "%d 10 128\n", trueDir->nfiles);

	for (int i = 0; i < trueDir->nfiles; ++i) {

		if(i != 0 && i%25 == 0){
			label++;
			printf("Label = %d\n", label);
		}

		

		/* Get image from dir */
		img = iftReadImageByExt(trueDir->files[i]->pathname);
		/* Extract HoG */
		feat = hog2(img);

		//printf("Feat.size = %d\n", feat->n);


		fprintf(fp, "%d %d ", i, label);

		/* Fill dataset */
		for (int f = 0; f < feat->n; f++) {
			dataset->sample[candIndex].feat[f] = feat->val[f];
			fprintf(fp, "%f ", feat->val[f]);
		}

		fprintf(fp, "\n");
		dataset->sample[candIndex].truelabel = label;

		printf("[%d] - %s\n", dataset->sample[candIndex].truelabel, trueDir->files[i]->pathname);

		candIndex++;



		iftDestroyFeatures(&feat);
		iftDestroyImage(&img);
	}

	fclose(fp);


	/* Write dataset to file. */
	iftWriteOPFDataSet(dataset, "generated-dataset");

	printf("Dataset generated in file: generated-dataset.\n");

	/* Free memory*/
	iftDestroyDataSet(&dataset);
	iftDestroyDir(&trueDir);

	return 0;
}
