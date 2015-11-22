#include "iftCharRecog.h"

int main(int argc, char** argv) {

	if (argc != 2) {
		printf("Usage: <img_path>\n");
		return 1;
	}

	iftAdjRel* A = iftRectangular(1.0, 10.0);

	iftImage* img = iftReadImageByExt(argv[1]);

	iftImage* sauv = binarizationBySauvola(img);

	iftWriteImageP2(sauv, "img_sauv.pgm");

	iftImage* closer =iftClose(sauv, A);
	iftImage* res =iftClose(closer, A);
	iftWriteImageP2(closer, "img_closer.pgm");

	iftDestroyImage(&img);
	iftDestroyImage(&sauv);

	iftDestroyAdjRel(&A);

	return 0;
}