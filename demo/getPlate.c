#include "ift.h"

int main(int argc, char *argv[]) {

	char roiFile[100];
	char mainFile[100];

	sprintf(mainFile, "%s", argv[1]);

	char* p = strtok(argv[1], "_");

	sprintf(roiFile, "%s_plate.pgm", p);

	iftImage *orig = iftReadImageByExt(mainFile);

	iftBoundingBox teste = iftObjectMinimumBoundingBox(orig);

	iftVoxel min, max;
	min.x = teste.min.x;
	min.y = teste.min.y;
	min.z = teste.min.z;

	max.x = teste.max.x;
	max.y = teste.max.y;
	max.z = teste.max.z;

	iftImage *roi = iftExtractROI(orig, min, max);

	iftWriteImageP2(roi, roiFile);

        iftDestroyImage(&orig);
        iftDestroyImage(&roi);

	return 0;
}
