#include "ift.h"

#define EPSILON 0.0001
#define MAX_INTERATION 1000

int main() {
	
	iftDataSet* dataSet = iftReadXYDataSet("points");

	iftDataSet* centers = iftKmeansInitCentroidsFromSamples(dataSet, 2);

	iftKmeansRun(0, dataSet, &centers, MAX_INTERATION, EPSILON);

	iftDraw2DFeatureSpace(centers, 0, 0);

	iftDestroyDataSet(&dataSet);
	iftDestroyDataSet(&centers);

	return 0;
}