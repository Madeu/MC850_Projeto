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

    HoGDesc* hog = generateHoG(norm_img, 2, 32, 32, 1);

    for (int i = 0; i < hog->sizeHist; i++) {
        printf("%lf ", hog->hist[i]);
    }

    putchar('\n');

    iftDestroyImage(&norm_img);
    destroyHoGDesc(hog);
}
