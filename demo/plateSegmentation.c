#include "iftSeedsGenerator.h"

iftVoxel getCandidateVoxel(char *filename, iftVoxel z){

	char candidateFile[100];

	char* p = strtok(filename, ".");

	sprintf(candidateFile, "../candidates/%s_candidate.txt", p);

	FILE *candidateCoord = fopen(candidateFile, "r");

	iftVoxel v;

	fscanf(candidateCoord, "%d %d", &v.x, &v.y);
	printf("%d %d\n", v.x, v.y);

	v.x = v.x + z.x;
	v.y = v.y + z.y;

	fclose(candidateCoord);

	return v;
}


void genSeed(char *filename, iftImage *img){

    iftAdjRel *A = iftCircular(3.0);

    iftColor color;
    color.val[0] = 1.0;
    color.val[1] = 1.0;
    color.val[2] = 1.0;
    iftColor pcolor = iftRGBtoYCbCr(color, 0);

	char objSeed[100];

	char* p = strtok(filename, ".");

	sprintf(objSeed, "../seeds/%s_objseed.pgm", p);

	//Metodo para pegar o centroide mais denso
	iftVoxel v = getBigCenter(img, 1000, 2);	

	iftDrawPoint(img, v, pcolor, A);

	iftWriteImageP2(img, objSeed);

	printf("Plot img %s\n", objSeed);


	iftVoxel aux = getCandidateVoxel(filename, v);

	writeSeedFile(filename, aux);


	iftDestroyImage(&img);
	iftDestroyAdjRel(&A);
}


int main(int argc, char *argv[]) {

    iftImage* orig;
    timer *t1 = NULL, *t2 = NULL;
    char outfile[100];


    iftDir* inputDir = iftLoadFilesFromDirectory(argv[1], "pgm");



    int i;
    for (i = 0; i < inputDir->nfiles; ++i) {

        orig = iftReadImageByExt(inputDir->files[i]->pathname);

	genSeed(iftBasename(inputDir->files[i]->pathname), orig);


    }

    return 0;
}
