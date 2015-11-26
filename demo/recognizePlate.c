#include "ift.h"
#include "iftCharRecog.h"
#include "iftSegmentation.h"
#include "iftCorrectPlate.h"

iftCandidate selectCandidate(iftImage *orig) {
	/* Sauvoula binarization to shape letters*/
	iftImage* sauv = binarizationBySauvola(orig);

	/* Sobel filter to mark horizontal edges.*/
	iftKernel *Kx = iftSobelXKernel2D();
	iftImage* sob = iftFastLinearFilter(sauv, Kx, 0);
	iftImage* sob_abs= iftAbs(sob);
	iftDestroyImage(&sauv);
	iftDestroyImage(&sob);

	/* Open filter on image to eliminate noize*/
	iftAdjRel* A = iftCircular(1.0);
	iftImage *filter_img = iftOpen(sob_abs, A);
	iftDestroyImage(&sob_abs);
	iftDestroyAdjRel(&A);

	iftCandidate candidate = computeDenserRegions(filter_img, orig);
	iftDestroyImage(&filter_img);

	return candidate;
}

int main(int argc, char const *argv[])
{
	if (argc < 3) {
		printf("usage:\n\t %s <img_source> <img _dest>\n", argv[0]);
		exit(1);
	}
	iftImage *orig = iftReadImageByExt(argv[1]);

	iftCandidate cand = selectCandidate(orig);

	iftWriteImageP2(cand.candidate, argv[2]);

	iftDestroyImage(&orig);
	iftDestroyImage(&(cand.candidate));

	return 0;
}