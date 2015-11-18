#include "iftSeedsGenerator.h"

int main(int argc, char *argv[]) {

	if (argc < 3) {
		printf("usage:\n\t %s <dir_source> <dir_dest> \n", argv[0]);
		exit(1);
	}
	

	iftAdjRel *A = iftCircular(2.0);

	iftColor color;
	color.val[0] = 1.0;
	color.val[1] = 1.0;
	color.val[2] = 1.0;
	iftColor pcolor = iftRGBtoYCbCr(color, 0);

	iftDir* inputDir = iftLoadFilesFromDirectory(argv[1], "pgm");
	char outfile[100];

	int i;
    	for (i = 0; i < inputDir->nfiles; ++i) {

		iftImage *img = iftReadImageByExt(inputDir->files[i]->pathname);

		iftVoxel v = getBigCenter(img, 10000, 2);

	
		sprintf(outfile, "%s/%s", argv[2], iftBasename(inputDir->files[i]->pathname));
		iftDrawPoint(img, v, pcolor, A);
		iftWriteImageP2(img, outfile);

		printf("Plot img %d\n", i);

		iftDestroyImage(&img);
 	}	

	iftDestroyAdjRel(&A);

	return 0;
}
