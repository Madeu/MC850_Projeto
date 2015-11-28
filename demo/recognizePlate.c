#include "ift.h"
#include "iftSegmentation.h"
#include "iftCorrectPlate.h"
#include "hog2.h"

iftCandidate *selectCandidate(iftImage *orig) {
	/* Sauvoula binarization to shape letters*/
	iftImage* sauv = binarizationBySauvola(orig);

	/* Sobel filter to mark horizontal edges.*/
	iftKernel *Kx = iftSobelXKernel2D();
	iftImage* sob = iftFastLinearFilter(sauv, Kx, 0);
	iftImage* sob_abs= iftAbs(sob);
	iftDestroyImage(&sauv);
	iftDestroyImage(&sob);
	iftDestroyKernel(&Kx);

	/* Open filter on image to eliminate noize*/
	iftAdjRel* A = iftCircular(1.0);
	iftImage *filter_img = iftOpen(sob_abs, A);
	iftDestroyImage(&sob_abs);
	iftDestroyAdjRel(&A);

	iftCandidate *candidates = computeDenserRegions(filter_img, orig);
	iftDestroyImage(&filter_img);

	return candidates;
}


int main(int argc, char const *argv[])
{
	char str[80];
	int descriptorSize = (HOG_N1 / HOG_N2 - HOG_N3 + 1)
			* (HOG_M1 / HOG_M2 - HOG_M3 + 1) * HOG_N3 * HOG_M3 * 9;

	float best_weight = -1.0;
	int better_ind = 0;
	int size = 30;

	if (argc < 3) {
		printf("usage:\n\t %s <img_source> <img _dest> <svm_path>\n", argv[0]);
		exit(1);
	}
	iftImage *orig = iftReadImageByExt(argv[1]);
	iftImage *plates[30];
	iftDataSet *dataset = iftCreateDataSet(30, descriptorSize);

	iftCandidate *cand = selectCandidate(orig);

	iftSVM *svm = iftReadSVM(argv[3]);

	for (int i = 0; i < 30; i++) {
		iftImage *plate = getCorrectedImage(orig, cand[i].candidate, cand[i].point);
		
		iftFeatures *feat = hog2(plate);

		for (int f = 0; f < feat->n; f++) {
			dataset->sample[i].feat[f] = feat->val[f];
			dataset->sample[i].id = i+1;
		}

		iftDestroyFeatures(&feat);
		plates[i] = plate;
	}

	iftSVMClassifyOVA(svm, dataset, TEST);


	for (int i = 0; i < 30; i++) {
		if (dataset->sample[i].label == 1) {
			printf("%d, %f\n", i, dataset->sample[i].weight);
		}

		if (dataset->sample[i].label == 1 && dataset->sample[i].weight > best_weight) {
			best_weight = dataset->sample[i].weight;
			better_ind = i;
		}
	}

	iftWriteImageP5(plates[better_ind], argv[2]);

	iftDestroyImage(&orig);
	for (int i = 0; i < 30; i++) {
		iftDestroyImage(&plates[i]);
		iftDestroyImage(&(cand[i].candidate));
	}
	free(cand);

	return 0;
}