//
// Created by Alan Peixinho on 9/3/15.
//

#include "iftHoG.h"

int main() {

    iftImage* img = iftReadImageByExt("lena.pgm");

    iftAdjRel *A = iftCircular(1.0);
    iftImage* norm_img = iftNormalizeImage(img, A, 4095);

    iftDestroyImage(&img);
    iftDestroyAdjRel(&A);

    genereteHoG(norm_img, 2, 128, 128, 1);

    iftDestroyImage(&norm_img);
    //destroyHoGDesc(hog);
}
