#include "ift.h"
#include <math.h>

typedef struct img_hog {
    iftImage *mag;
    iftMatrix *orient;
}imgHoG;

typedef struct hog_desc {
    float *hist;
    int size_hist;
	int numDirect;    
} HoGDesc;

typedef struct block {
	int num_cels;
	int xsize_cels;
	int ysize_cels;
	HoGDesc *hogs;
} Block;

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

imgHoG* createImgHoG(iftImage *img) {
    double gx, gy, res;

    iftImage* gaussx = imgConv(img, "gauss_x");
    iftImage* gaussy = imgConv(img, "gauss_y");
    iftImage* gauss = iftAdd(gaussx, gaussy);

    iftMatrix* orient = iftCreateMatrix(img->xsize, img->ysize);

    printf("%d\n", (2 == 1) ? abs(-10) : -10);

    for (int p = 0; p < gauss->n; ++p) {

        gx = (gauss->val[p] != 0) ? gaussx->val[p]/abs(gauss->val[p]) : 0;
        gy = (gauss->val[p] != 0) ? gaussy->val[p]/abs(gauss->val[p]) : 0;

        res = (180/PI);
        if (gy < 0){
            res = 360 - res;
        }

        orient->val[p] = res;

    }

    iftDestroyImage(&gaussx);
    iftDestroyImage(&gaussx);


    imgHoG* hogs = (imgHoG *) malloc(sizeof(imgHoG));
    hogs->mag = gauss;
    hogs->orient = orient;

    return hogs;
}

void destroyImgHoG(imgHoG* imghog) {
    iftDestroyImage(&(imghog->mag));
    iftDestroyMatrix(&(imghog->orient));

    free(imghog);
}