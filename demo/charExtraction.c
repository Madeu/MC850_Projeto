#include "iftSeedsGenerator.h"
#include "iftSelectCandidates.h"
#include "iftExtractFeatures.h"

iftImage *iftRemoveSmallComponents(iftImage *img, int minVolume) {

    int p, i;
    int max = iftMaximumValue(img);
    int *volume = iftAllocIntArray(max + 1);
    int *labels = iftAllocIntArray(max + 1);

    for (p = 0; p < img->n; ++p) {
        if (img->val[p] > 0)
            volume[img->val[p]]++;
    }

   /* for (i = 1; i <= max; ++i) {
        printf("Volume label %d = %d\n", i, volume[i]);
    }*/

    int nlabels = 1;
    for (i = 1; i <= max; ++i) {
        if (volume[i] >= minVolume)
            labels[i] = nlabels++;
        else
            labels[i] = 0;
    }

    for (p = 0; p < img->n; ++p) {
        img->val[p] = labels[img->val[p]];
    }

    free(labels);
    free(volume);

	return img;
}



int main(int argc, char *argv[])
{

	iftImage *gt = iftReadImageByExt(argv[1]);

	char* mainFile = strtok(argv[2], ".");

	iftVoxel roi0, roi1;
	roi0.x = 0;
	roi0.y = gt->ysize/4;
	roi0.z = 0;

	roi1.x = gt->xsize-1;
	roi1.y = gt->ysize-1;
	roi1.z = 0;


	iftImage *roi = iftExtractROI(gt, roi0, roi1);

	iftImage *output;

	int t = iftOtsu(roi);
        int max = iftMaximumValue(roi);

        output = iftThreshold(roi, t, max, 255);

	iftAdjRel *A = iftCircular(1.5);

	iftImage *aux = iftDilate(output, A);

	for(int i = 0; i < aux->n; i++){
		if(aux->val[i] == 0)
			aux->val[i] = 1;
		else
			aux->val[i] = 0;

	}

	iftImage *aux1 = iftFastLabelComp(aux, A);

	iftImage *aux2 = iftRemoveSmallComponents(aux1, 100);

	max = iftMaximumValue(aux2);

	for (int i = 1; i <= max; ++i) {
		iftImage *seg = iftCreateBoundingBox2D(roi, aux2, i) ;

		char outfile[100];
		sprintf(outfile, "%s_char_%d.pgm", mainFile, i);

		if(seg != NULL){
			t = iftOtsu(seg);

         		iftWriteImageP5(seg, outfile);
		}
	}

	char outfile[100];
	sprintf(outfile, "%s_labeled.pgm", mainFile);

	iftWriteImageP2(aux2, outfile);

	return 0;
}
