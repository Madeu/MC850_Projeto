#include "iftSeedsGenerator.h"

int main() {
	iftImage* img = iftReadImageByExt("lena.pgm");
	iftVoxel u[50];
	int size = readPoint("points.txt", u);

	iftDataSet* centers = generateCentroids(u, size);

	iftWriteImageP2(img, "nova_lena.pgm");
	iftDestroyDataSet(&centers);

	return 0;
}