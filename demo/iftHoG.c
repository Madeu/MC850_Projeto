//
// Created by Alan Peixinho on 9/3/15.
//

#include "iftHoG.h"

int main() {

    iftImage* img = iftReadImageByExt("lena.pgm");
    iftAdjRel *A = iftCircular(1.0);
    iftImage* norm_img = iftNormalizeImage(img, A, 4095);
    
    iftImage* gaussx = imgConv(img, "gauss_x");
    iftImage* gaussy = imgConv(img, "gauss_y");

    iftDestroyImage(&norm_img);
    iftDestroyImage(&gaussx);
    iftDestroyImage(&gaussy);
    iftDestroyImage(&img);
    iftDestroyImage(&newImg);
}
