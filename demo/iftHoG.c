//
// Created by Alan Peixinho on 9/3/15.
//

#include "iftHoG.h"
#include <math.h>

int main() {

    iftImage* img = iftReadImageByExt("lena.pgm");
    iftAdjRel *A = iftCircular(1.0);
    iftImage* norm_img = iftNormalizeImage(img, A, 4095);

    iftDestroyImage(&img);
    iftDestroyAdjRel(&A);

    iftImage* cut = cutImg(norm_img, 200, 200, 10, 10);

    imgGauss* hog = createImgGauss(cut);
    iftPrintMatrix(hog->orient);

    for(int k = 0; k < 4; k++) {
        int x = k%2; int y = k/2;
        double centerx = ceil(5.0/2.0)+(x*5), centery = ceil(5.0/2.0)+(y*5);
        int p = iftGetMatrixIndex(hog->orient, 0, 0);
        printf("%lf\n", ceil(hog->orient->val[p]/45));
    }

    iftPrintMatrix(hog->orient);

    iftDestroyImage(&norm_img);
    destroyImgGauss(hog);
    iftDestroyImage(&cut);
}
