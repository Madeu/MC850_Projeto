#include "ift.h"

typedef struct gauss {
    iftImage *mag;
    iftMatrix *orient;
}Gaussian;

typedef struct img_hog {
    int *hist;
    int size_hist;
    
} imgHoG;

iftImage* cutImg(iftImage *img, int pos_x, int pos_y, int width, int height) {

    iftImage* newImg = iftCreateImage(width, height, img->zsize);
    int ind = 0;

    for (int i = pos_x; i < pos_x+width; i++) {
        for (int j = pos_y; j < pos_y+height; j++){
            int p = j*img->xsize + i;
            iftVoxel v = iftGetVoxelCoord(img, p);

            if (iftValidVoxel(img, v)) {
                newImg->val[ind] = img->val[p];
                ind++;
            }
        }
    }

    return newImg;

}

iftImage* imgConv(iftImage *img, char *filename) {

    iftMatrix* matrix = iftReadMatrix(filename);
    
    iftAdjRel* adj = iftRectangular(matrix->ncols, matrix->nrows);

    iftImage* newImg = iftCreateImage(img->xsize, img->ysize, img->zsize);

    for (int p = 0; p < img->n; ++p) {//foreach pixel in the image
        iftVoxel v = iftGetVoxelCoord(img, p);//gets the multidimensional coordinate using the unidimensional index
        int s = 0;
        for (int i = 0; i < adj->n; ++i) {//foreach neighbor voxel in the adjacency
            iftVoxel u = iftGetAdjacentVoxel(adj, v, i);
            if(iftValidVoxel(img, u)) {//check if it is a valid voxel (is it inside the image?)
                int q = iftGetVoxelIndex(img, u);
                s+= matrix->val[i] *img->val[q];
            }
        }
        newImg->val[p] = s/adj->n;
    }

    iftPrintMatrix(matrix);
    iftDestroyMatrix(&matrix);
    
    return newImg;
}