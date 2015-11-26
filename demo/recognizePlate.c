#include "ift.h"
#include "iftCharRecog.h"
#include "iftSegmentation.h"
#include "iftCorrectPlate.h"

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

void segmentCandidates(iftCandidate *cand, iftImage *img, const char *name) {
	char str[80];


	
}

int main(int argc, char const *argv[])
{
	char str[80];

	if (argc < 3) {
		printf("usage:\n\t %s <img_source> <img _dest>\n", argv[0]);
		exit(1);
	}
	iftImage *orig = iftReadImageByExt(argv[1]);

	iftCandidate *cand = selectCandidate(orig);

	for (int i = 0; i < 5; i++) {
		iftImage *plate = getCorrectedImage(orig, cand[i].candidate, cand[i].point);
		sprintf(str, "%s-%d", argv[2], i);
		iftWriteImageP5(plate, str);

		iftDestroyImage(&plate);
	}

	iftDestroyImage(&orig);
	for (int i = 0; i < 5; i++) {
		iftDestroyImage(&(cand[i].candidate));
	}
	free(cand);

	return 0;
}