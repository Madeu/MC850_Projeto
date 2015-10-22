#include "ift.h"
#include <math.h>

typedef struct img_gauss {
    iftImage *mag;
    iftMatrix *orient;
}imgGauss;

typedef struct hog_desc {
    float *hist;
    int sizeHist;
	int numDirect;    
} HoGDesc;

typedef struct block {
	int numCels;
	int xsizeCels;
	int ysizeCels;
	HoGDesc *hogs;
} Block;


HoGDesc* createHoGDesc(int sizeHist, int numDirect) {
    float* hist = (float *) malloc(sizeHist*sizeof(float));

    HoGDesc* hog = (HoGDesc *) malloc(sizeof(HoGDesc));
    hog->sizeHist = sizeHist;
    hog->numDirect = numDirect;
    hog->hist = hist;

    for (int i = 0; i < sizeHist; i++)
        hog->hist[i] = 0;

    return hog;
}

void destroyHoGDesc(HoGDesc* hog){
    free(hog->hist);
    free(hog);
}

Block* createBlock(int xsizeCels, int ysizeCels, int numCels) {
    Block* block = (Block *) malloc(sizeof(Block));
    block->numCels = numCels;
    block->xsizeCels = xsizeCels;
    block->ysizeCels = ysizeCels;
    block->hogs = createHoGDesc(9*numCels, 9);

    return block;
}

void destroyBlock(Block* block) {
    destroyHoGDesc(block->hogs);
    free(block);
}

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

    iftDestroyMatrix(&matrix);
    iftDestroyAdjRel(&adj);
    
    return newImg;
}

imgGauss* createImgGauss(iftImage *img) {
    double gx, gy, res;

    iftImage* gaussx = imgConv(img, "gauss_x");
    iftImage* gaussy = imgConv(img, "gauss_y");
    iftImage* gauss = iftAdd(gaussx, gaussy);

    iftMatrix* orient = iftCreateMatrix(img->xsize, img->ysize);

    for (int p = 0; p < gauss->n; ++p) {

        gx = (gauss->val[p] != 0) ? gaussx->val[p]/gauss->val[p] : 0;
        gy = (gauss->val[p] != 0) ? gaussy->val[p]/gauss->val[p] : 0;

        res = (180/PI)*acos(gx);
        if (gy < 0){
            res = 360 - res;
        }

        //printf("%f, %f, %f \n", gx, gy, res/45);

        orient->val[p] = res;

    }

    iftDestroyImage(&gaussx);
    iftDestroyImage(&gaussx);


    imgGauss* igauss = (imgGauss *) malloc(sizeof(imgGauss));
    igauss->mag = gauss;
    igauss->orient = orient;

    return igauss;
}

void destroyImgGauss(imgGauss* igauss) {
    iftDestroyImage(&(igauss->mag));
    iftDestroyMatrix(&(igauss->orient));

    free(igauss);
}

void genereteHoG(iftImage* img, int cels, int xsizeCels, int ysizeCels, int step) {
    int numCelsW = img->xsize/xsizeCels, numCelsH = img->ysize/ysizeCels,
    numBlockW = numCelsW - (cels-step), numBlockH = numCelsH - (cels-step),
    hist_size = cels*cels* 9 * numBlockW * numBlockH;
    iftImage *cutimg;
    int x, y, sizexWindow, sizeyWindow;

    for (int i = 0; i < numBlockW; i++) {
        for (int j = 0; j < numBlockH; j++) {
            x = i*step*xsizeCels; y = j*step*ysizeCels;
            cutimg = cutImg(img, x, y, cels*sizexWindow, cels*sizeyWindow);
            iftDestroyImage(&cutimg);
        }
    }

}