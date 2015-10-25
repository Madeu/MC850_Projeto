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

double distPoint(int xA, int yA, int xB, int yB) {
    return sqrt(pow((xA - xB), 2) + pow((yA - yB), 2));
}

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

iftMatrix* imgConv(iftImage *img, char *filename) {

    iftMatrix* matrix = iftReadMatrix(filename);
    
    iftAdjRel* adj = iftRectangular(matrix->ncols, matrix->nrows);

    iftMatrix* newImg = iftCreateMatrix(img->xsize, img->ysize);

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

iftMatrix* addMatrix(iftMatrix* matrixA, iftMatrix* matrixB) {
    iftMatrix* matrix = iftCreateMatrix(matrixA->ncols, matrixA->nrows);

    for (int i = 0; i < matrix->n; i++)
        matrix->val[i] = matrixA->val[i] + matrixB->val[i];

    return matrix;
}

iftImage* matrixToImg(iftMatrix* matrix) {
    iftImage* img = iftCreateImage(matrix->ncols, matrix->nrows, 0);

    for (int i = 0; i < img->n; i++)
        img->val[i] = (int) matrix->val[i];

    return img;
}

imgGauss* createImgGauss(iftImage *img) {
    double gx, gy, res;

    iftMatrix* gaussx = imgConv(img, "gauss_x");
    iftMatrix* gaussy = imgConv(img, "gauss_y");

    iftMatrix* orient = iftCreateMatrix(img->xsize, img->ysize);

    for (int p = 0; p < orient->n; p++){

        double mag = gaussx->val[p] + gaussy->val[p];
        double gx = gaussx->val[p]/mag;
        double gy = gaussy->val[p]/mag;
        double res = (180/PI)*acos(gx);

        if(gy < 0) {
            res = 360 - res;
        }

        orient->val[p] = res;
    }

    iftMatrix* gauss = addMatrix(gaussx, gaussy);
    iftImage* gauss_img = matrixToImg(gauss);
    iftDestroyMatrix(&gaussx);
    iftDestroyMatrix(&gaussx);
    iftDestroyMatrix(&gauss);

    imgGauss* igauss = (imgGauss *) malloc(sizeof(imgGauss));
    igauss->mag = gauss_img;
    igauss->orient = orient;

    return igauss;
}

void destroyImgGauss(imgGauss* igauss) {
    iftDestroyImage(&(igauss->mag));
    iftDestroyMatrix(&(igauss->orient));

    free(igauss);
}

HoGDesc* getHistogram(iftImage* img, int cels, int celsizex, int celsizey) {
    int imgNum = cels*cels, sizeHist = cels*cels*9;
    imgGauss* igauss = createImgGauss(img);
    HoGDesc* hog = createHoGDesc(9*imgNum, 9) ;
    
    for(int k = 0; k < imgNum; k++) {
        int x = k%cels; int y = k/cels;
        for (int i = x*celsizex; i < (x+1)*celsizex; i++) {
            for (int j = y*celsizey; j < (y+1)*celsizey; j++) {
                //TODO : Interpolação.
                int p = iftGetMatrixIndex(igauss->orient, i, j);
                int t = (int) floor((igauss->orient->val[p])/45);
                hog->hist[9*k + (abs(t) < 9 ? t : 8)] ++;
            }
        }
    }

    destroyImgGauss(igauss);
    return hog;
}

HoGDesc* generateHoG(iftImage* img, int cels, int celsizex, int celsizey, int step) {
    int blockSize = cels*cels, numCelsW = img->xsize/celsizex, numCelsH = img->ysize/celsizey;
    HoGDesc* hogRes = createHoGDesc(9*blockSize*(numCelsW - (cels-step))*(numCelsH - (cels-step)), 9);

    for (int i = 0; i < numCelsW - (cels-step); i+=step) {
        for (int j = 0; j < numCelsH - (cels-step); j+=step) {
            int x = i*celsizex; int y = j*celsizey;
            iftImage* cut = cutImg(img, x, y, x+(cels*celsizex), y+(cels*celsizey));
            HoGDesc* hog = getHistogram(cut, cels, celsizex, celsizey);

            int posHist = (9*blockSize)*(i*(numCelsH - (cels-step))+j);
            for (int t = 0; t < 9*blockSize; t++) {
                hogRes->hist[posHist + i] = hog->hist[i];
            }

            destroyHoGDesc(hog);
            iftDestroyImage(&cut);
        }
    }

    return hogRes;
}