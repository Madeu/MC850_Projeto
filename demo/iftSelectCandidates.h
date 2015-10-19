//
// Created by Alan Peixinho on 8/31/15.
//

#ifndef IFT_IFTSELECTCANDIDATES_H
#define IFT_IFTSELECTCANDIDATES_H

#include "iftMathMorph.h"
#include "iftMetrics.h"
#include "iftSegmentation.h"


#define MIN_VOLUME 1500

/**
 * @brief Remove components with less than minVolume pixels.
 */
void iftRemoveSmallComponents(iftImage *img, int minVolume) {

    int p, i;
    int max = iftMaximumValue(img);
    int *volume = iftAllocIntArray(max + 1);
    int *labels = iftAllocIntArray(max + 1);

    for (p = 0; p < img->n; ++p) {
        if (img->val[p] > 0)
            volume[img->val[p]]++;
    }

    int nlabels = 1;
    for (i = 1; i <= max; ++i) {
        if (volume[i] < minVolume)
            labels[i] = nlabels++;
        else
            labels[i] = 0;
    }

    for (p = 0; p < img->n; ++p) {
        img->val[p] = labels[img->val[p]];
    }

    free(labels);
    free(volume);
}


iftImage *binarizationBySauvola(iftImage *orig){
    
    iftImage *aux;
    int arrayIndex = 0;
    
    iftVoxel v;
    v.x = 0;
    v.y = 0;
    v.z = 0;
        
    int tileSize = 8; // Common to 352(xsize) and 240(ysize).
    int numWindowsX = orig->xsize/tileSize;
    int numWindowsY = orig->ysize/tileSize;
    
    // Sauvola parameters.
    int R = 128;
    float k = 0.5;
        
    // Array that stores pixels' values to compute local threshold.
    float pixelArray[tileSize*tileSize];
    
    // Binarized image.    
    aux = iftCreateImage(orig->xsize, orig->ysize, orig->zsize);

    // Loop to slide window through the image.
    for(int xWin = 0; xWin < numWindowsX; xWin++){
        for(int yWin = 0; yWin < numWindowsY; yWin++){
            
            // Loop to get values of all pixels of the tile.
            for(int i = tileSize*xWin; i < tileSize*(xWin+1); i++){
                for(int j = tileSize*yWin; j < tileSize*(yWin+1); j++){
                    v.x = i;
                    v.y = j;
                    
                    int coord = iftGetVoxelIndex(orig, v);
                    pixelArray[arrayIndex] = orig->val[coord];
                    
                    arrayIndex++;
                }                
            }
            
            float mean = iftMeanFloatArray(pixelArray, tileSize*tileSize);
            float stdDev = iftStddevFloatArray(pixelArray, tileSize*tileSize);   
                
            int T = mean * (1 + k * (stdDev/R - 1));  
                
            //printf("Threshold: %d\n", T);
                    
            arrayIndex = 0;
            
            // Binarize image
            for(int i = tileSize*xWin; i < tileSize*(xWin+1); i++){
                for(int j = tileSize*yWin; j < tileSize*(yWin+1); j++){
                    v.x = i;
                    v.y = j;
                    
                    int coord = iftGetVoxelIndex(orig, v);
                    if(orig->val[coord] < T){
                        aux->val[coord] = 4095;                        
                    } else{
                        aux->val[coord] = 0;
                    }                     
                }                
            }  
        }
    } 
    
    return aux;
    
}

iftImage *computeDenserRegions(iftImage *orig, iftImage *plateImage, char *filename){
    
    iftVoxel v;
    v.x = 0;
    v.y = 0;
    v.z = 0;
    
    iftVoxel maxVoxel;
    maxVoxel.x = 0;
    maxVoxel.y = 0;
    maxVoxel.z = 0;
    
    int xSize = orig->xsize;
    int ySize = orig->ysize;
    
    int tileXSize = 120;
    int tileYSize = 45;
    
    int startXPixel = 0;
    int startYPixel = 0;
    
    int count;
    
    float maxPercentage = 0.0;
    
    for(int x = startXPixel; x < xSize; x=x+4){
        
        if(x >= xSize || tileXSize+x >= xSize){
            break;
        }
        
        for(int y = startYPixel; y < ySize; y=y+4){
            
            if(y >= ySize || tileYSize+y >= ySize){
                break;
            }
            
            count = 0;
            
            float percentage = 0.0;
            
            /*for(int i = x; i < tileXSize+x; i++){                
                for(int j = y; j < tileYSize+y; j++){
                    v.x = i;
                    v.y = j;
                    
                    int coord = iftGetVoxelIndex(orig, v);
                    if(orig->val[coord] == 4095){
                        count++;
                    }  
                    
                }
            }*/
            
            // Considering a 15x15 window
            // 9 = número de janelas em X.
            // 3 = número de janelas em Y.
            for(int i = 0; i < 8; i++){
                for(int j = 0; j < 3; j++){
                    
                    count = 0;
                    
                    for(int xCoord = x + 15*i; xCoord < (x + 15*i+15); xCoord++){
                        for(int yCoord = y + 15*j; yCoord < (y + 15*j+15); yCoord++){
                            v.x = xCoord;
                            v.y = yCoord;
                            
                            //printf("Pixel [%d][%d] -- %d|%d -- i = %d; j = %d\n", xCoord, yCoord, x, y, i, j);
                    
                            int coord = iftGetVoxelIndex(orig, v);
                            if(orig->val[coord] >= 2048){
                                count++;
                            } 
                            
                        }
                    }
                    
                    //printf("--------------- [%d][%d] = %d\n", i, j,count);
                    
                    percentage = percentage + (float)count/225;
                    
                }
            }
            
            if(percentage > maxPercentage){
                if(y >= orig->ysize/2){
                    maxPercentage = percentage;
                
                    maxVoxel.x = x;
                    maxVoxel.y = y;
                }
            }
            
            
            //printf("Region starting in (%d,%d): %f \n", x, y, percentage);
            
        }
    }
        
    printf("Max percentage: %f @ [%d, %d]\n", maxPercentage, maxVoxel.x, maxVoxel.y);
    
    
    
 iftImage *teste = iftCreateImage(orig->xsize, orig->ysize, orig->zsize);
    
    // Set pixel's value inside plate region to 4095.
    for(int xCoord = maxVoxel.x; xCoord < (maxVoxel.x + 135); xCoord++){
        for(int yCoord = maxVoxel.y; yCoord < (maxVoxel.y + 45); yCoord++){
            v.x = xCoord;
            v.y = yCoord;
            
            //printf("Pixel [%d][%d] -- %d|%d -- i = %d; j = %d\n", xCoord, yCoord, x, y, i, j);
                    
            int coord = iftGetVoxelIndex(plateImage, v);
            teste->val[coord] = plateImage->val[coord];
        }
    }
    
    iftWriteImageP2(teste, filename);
    
    
    
    // Set pixel's value outside plate region to 0.
    for(int x = 0; x < orig->xsize; x++ ){
        for(int y = 0; y < orig->ysize; y++){
            v.x = x;
            v.y = y;
            
            int coord = iftGetVoxelIndex(orig, v);
            orig->val[coord] = 0;
            
        }
    }
    
    
    // Set pixel's value inside plate region to 4095.
    for(int xCoord = maxVoxel.x; xCoord < (maxVoxel.x + tileXSize); xCoord++){
        for(int yCoord = maxVoxel.y; yCoord < (maxVoxel.y + tileYSize); yCoord++){
            v.x = xCoord;
            v.y = yCoord;
            
            //printf("Pixel [%d][%d] -- %d|%d -- i = %d; j = %d\n", xCoord, yCoord, x, y, i, j);
                    
            int coord = iftGetVoxelIndex(orig, v);
            orig->val[coord] = 4095;
        }
    }
    
    
    
    
    //orig = iftAddRectangularBoxFrame(orig, v.x, v.y, 0, 255);
    
    
    
    return orig;    
    
    
}

iftImage *selectCandidates(iftImage *orig, char *filename) {
   
    iftAdjRel *A = NULL;  
    iftKernel *Kx = NULL;
    iftKernel *Ky = NULL;
    
    iftImage *aux[4];
    
    char outfile[70];
    
    aux[0] = binarizationBySauvola(orig);
    
    Kx = iftSobelXKernel2D();
    aux[1] = iftFastLinearFilter(aux[0], Kx, 0);
    aux[2] = iftAbs(aux[1]);    
    
    iftDestroyImage(&aux[0]);
    iftDestroyImage(&aux[1]);
    
    
    
    A = iftCircular(1.0);
    aux[3] = iftOpen(aux[2], A);
    iftDestroyImage(&aux[2]);
    
    
    
    aux[4] = iftAlphaPooling(aux[3], A, 4, 2);
    iftDestroyImage(&aux[3]);
    iftDestroyAdjRel(&A);
    
    //return aux[4];
    
    //sprintf(outfile, "%s/%s", "/home/luis/NetBeansProjects/PlateDetection/candidates_results", filename);
    
    aux[0] = computeDenserRegions(aux[4], orig, filename);
    
    int max = iftMaximumValue(aux[0]);
    printf("Maximum value: %d\n", max);   
    
    
    return aux[0];
    
    
    
    
    /*A = iftCircular(1.0);
    aux[3] = iftFastLabelComp(aux[4], A);

    //iftRemoveSmallComponents(aux[3], MIN_VOLUME);

    iftImage* final = iftAddRectangularBoxFrame(aux[3], 4, 0, 0, 0);

    iftDestroyImage(&aux[0]);
    //iftDestroyImage(&aux[1]);
    iftDestroyImage(&aux[2]);
    iftDestroyImage(&aux[3]);

    iftDestroyAdjRel(&A);
    //iftDestroyKernel(&K);
    
    return final;*/
    
    
    
    
    
    
    
    /*
    aux[3] = iftAnd(aux[2], aux[0]);
    
    iftDestroyImage(&aux[0]);
    iftDestroyImage(&aux[2]);
    
    
    return  aux[3];*/
    /*
    A = iftCircular(5.0);
    aux[3] = iftFastLabelComp(aux[0], A);
    
    iftRemoveSmallComponents(aux[3], 1000);
    
    iftDestroyAdjRel(&A);
    iftDestroyImage(&aux[0]);
    /*sprintf(outfile, "%s/%s", "/home/luis/NetBeansProjects/PlateDetection/_candidates", iftBasename(filename));
    iftWriteImageP2(aux[2], outfile);*/
    
    //return aux[3];
    
    
    
    /*
    iftImage *aux[4];
    iftKernel *K = NULL;
    iftAdjRel *A = NULL;
    A = iftCircular(5.0);
    aux[0] = iftNormalizeImage(orig, A, 4095);

    aux[1] = iftCloseBasins(aux[0]);
    return aux[1];
    
    aux[2] = iftSub(aux[1], aux[0]);
    iftDestroyImage(&aux[1]);

    K = iftSobelXKernel2D();
    aux[1] = iftFastLinearFilter(orig, K, 0);
     
    aux[0] = iftAbs(aux[1]);
    
    iftDestroyImage(&aux[1]);
    
    int max = iftMaximumValue(aux[0]);
    int t = iftOtsu(aux[0]);

    aux[2] = iftThreshold(aux[0], t, max, 255);
    
    iftDestroyImage(&aux[0]);
    

    
    return aux[2];
      
    /* 
    iftDestroyImage(&aux[0]);
    iftDestroyImage(&aux[2]);
    iftDestroyAdjRel(&A);
    aux[2] = iftAbs(aux[1]);
    A = iftRectangular(10, 3);
    aux[0] = iftAlphaPooling(aux[2], A, 4, 2);
    
    return aux[0];*/

}

/* Fluxo original*/
/* 
    iftImage *aux[4];
    iftKernel *K = NULL;
    iftAdjRel *A = NULL;

    A = iftCircular(5.0);
    aux[0] = iftNormalizeImage(orig, A, 4095);

    aux[1] = iftCloseBasins(aux[0]);
    aux[2] = iftSub(aux[1], aux[0]);
    iftDestroyImage(&aux[1]);

    K = iftSobelXKernel2D();
    aux[1] = iftFastLinearFilter(aux[2], K, 0);
    iftDestroyImage(&aux[0]);
    iftDestroyImage(&aux[2]);
    iftDestroyAdjRel(&A);
    aux[2] = iftAbs(aux[1]);
    A = iftRectangular(10, 3);
    aux[0] = iftAlphaPooling(aux[2], A, 4, 2);

    iftDestroyImage(&aux[2]);
    int max = iftMaximumValue(aux[0]);
    int t = iftOtsu(aux[0]);

    aux[2] = iftThreshold(aux[0], t, max, 255);

    A = iftCircular(2.0);
    aux[3] = iftFastLabelComp(aux[2], A);

    iftRemoveSmallComponents(aux[3], MIN_VOLUME);

    iftImage* final = iftAddRectangularBoxFrame(aux[3], 4, 0, 0, 0);

    iftDestroyImage(&aux[0]);
    iftDestroyImage(&aux[1]);
    iftDestroyImage(&aux[2]);
    iftDestroyImage(&aux[3]);

    iftDestroyAdjRel(&A);
    iftDestroyKernel(&K);

    return final;
 */


#endif //IFT_IFTSELECTCANDIDATES_H
