#include "iftSeedsGenerator.h"

int main(int argc, char *argv[]) {

	if (argc < 3) {
		printf("usage:\n\t %s <img_source> <img _dest> \n", argv[0]);
		exit(1);
	}
	

	iftAdjRel *A = iftCircular(3.0);

	iftColor color;
	color.val[0] = 1.0;
	color.val[1] = 1.0;
	color.val[2] = 1.0;
	iftColor pcolor = iftRGBtoYCbCr(color, 0);

	iftImage *img = iftReadImageByExt(argv[1]);	

	//Metodo para pegar o centroide mais denso
	iftVoxel v = getBigCenter(img, 1000, 2);

	FILE *candidateCoord = fopen("../candidates/0006.pgm.txt", "r");

	int x, y;

	fscanf(candidateCoord, "%d %d", &x, &y);
	printf("%d %d\n", x, y);

	fclose(candidateCoord);

	iftVoxel aux;
	aux.x = v.x + x;
	aux.y = v.y + y;
	aux.z = v.z;

	writeSeedFile(aux);

	iftDrawPoint(img, v, pcolor, A);

	iftWriteImageP2(img, argv[2]);

	printf("Plot img %s\n", argv[2]);

	iftDestroyImage(&img);
	iftDestroyAdjRel(&A);

	return 0;
}
