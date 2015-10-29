//
// Created by Alan Peixinho on 8/31/15.
//

#ifndef IFT_IFTSELECTCANDIDATES_H
#define IFT_IFTSELECTCANDIDATES_H


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
    int xWin;
    int yWin;
    for(xWin = 0; xWin < numWindowsX; xWin++){
        for(yWin = 0; yWin < numWindowsY; yWin++){
            
            // Loop to get values of all pixels of the tile.
            int i;
            int j;
            
            for(i = tileSize*xWin; i < tileSize*(xWin+1); i++){
                for(j = tileSize*yWin; j < tileSize*(yWin+1); j++){
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
            for(i = tileSize*xWin; i < tileSize*(xWin+1); i++){
                for(j = tileSize*yWin; j < tileSize*(yWin+1); j++){
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

iftImage *binarizationByNiblack(iftImage *orig){
    
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
    float k = -0.2;
        
    // Array that stores pixels' values to compute local threshold.
    float pixelArray[tileSize*tileSize];
    
    // Binarized image.    
    aux = iftCreateImage(orig->xsize, orig->ysize, orig->zsize);

    // Loop to slide window through the image.
    int xWin;
    int yWin;
    for(xWin = 0; xWin < numWindowsX; xWin++){
        for(yWin = 0; yWin < numWindowsY; yWin++){
            
            // Loop to get values of all pixels of the tile.
            int i;
            int j;
            for(i = tileSize*xWin; i < tileSize*(xWin+1); i++){
                for(j = tileSize*yWin; j < tileSize*(yWin+1); j++){
                    v.x = i;
                    v.y = j;
                    
                    int coord = iftGetVoxelIndex(orig, v);
                    pixelArray[arrayIndex] = orig->val[coord];
                    
                    arrayIndex++;
                }                
            }
            
            float mean = iftMeanFloatArray(pixelArray, tileSize*tileSize);
            float stdDev = iftStddevFloatArray(pixelArray, tileSize*tileSize);   
                
            int T = mean + k * stdDev;  
                
            //printf("Threshold: %d\n", T);
                    
            arrayIndex = 0;
            
            // Binarize image
            for(i = tileSize*xWin; i < tileSize*(xWin+1); i++){
                for(j = tileSize*yWin; j < tileSize*(yWin+1); j++){
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
    
    iftVoxel maxVoxel_1;
    maxVoxel_1.x = 0;
    maxVoxel_1.y = 0;
    maxVoxel_1.z = 0;
    
    int xSize = orig->xsize;
    int ySize = orig->ysize;
    
    int tileXSize = 120;
    int tileYSize = 45;
    
    int startXPixel = 0;
    int startYPixel = 0;
    
    int stride = 4;
    
    float maxPercentage_1 = 0.0;
    
    int x;
    for(x = startXPixel; x < xSize; x=x+stride){
        
        if(x >= xSize || tileXSize+x >= xSize){
            break;
        }
        
        int y;
        for(y = startYPixel; y < ySize; y=y+stride){
            
            if(y >= ySize || tileYSize+y >= ySize){
                break;
            }
           
            
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
            }
            
            printf(">>>>>>>>>>>>>> [%d][%d] = %f || %d\n", x, y,percentage);*/
            
            
            // Considering a 15x15 window
            // 8 = número de janelas em X.
            // 3 = número de janelas em Y.
            int i;
            int j;
            for(i = 0; i < 8; i++){
                for(j = 0; j < 3; j++){
                    
                    int count = 0;
                    
                    float values[225];

            	   int xCoord;
            	   int yCoord;
                    
                    for(xCoord = x + 15*i; xCoord < (x + 15*i+15); xCoord++){
                        for(yCoord = y + 15*j; yCoord < (y + 15*j+15); yCoord++){
                            v.x = xCoord;
                            v.y = yCoord;
                            
                            //printf("Pixel [%d][%d] -- %d|%d -- i = %d; j = %d\n", xCoord, yCoord, x, y, i, j);
                    
                            int coord = iftGetVoxelIndex(orig, v);
                           /* if(orig->val[coord] >= 2048){
                                count++;
                            } */
                            values[count] = orig->val[coord];
                            count++;                                                       
                        }
                    }
                    
                    //percentage = percentage + (float)count/225;                    
                    
                    float stddev = iftStddevFloatArray(values, 225);
                    
                    //printf("--------------- [%d][%d] = %d\n", i, j,count);
                    
                    percentage = percentage + stddev;
                }
            }
            
            if(y >= orig->ysize/2){
                if(percentage >= maxPercentage_1){

                    maxPercentage_1 = percentage;                
                    maxVoxel_1.x = x;
                    maxVoxel_1.y = y;
                }
            }         
        }
    }
        
    printf("Max percentage: %f @ [%d, %d]\n", maxPercentage_1, maxVoxel_1.x, maxVoxel_1.y);
    
    
    //--------------------------------------------------------------------------
    // Saves selected region.
    iftImage *teste = iftCreateImage(orig->xsize, orig->ysize, orig->zsize);    
    
    int xCoord;
    int yCoord;
    for(xCoord = maxVoxel_1.x; xCoord < (maxVoxel_1.x + 135); xCoord++){
        for(yCoord = maxVoxel_1.y; yCoord < (maxVoxel_1.y + 45); yCoord++){
            v.x = xCoord;
            v.y = yCoord;
                    
            int coord = iftGetVoxelIndex(plateImage, v);
            teste->val[coord] = plateImage->val[coord];
        }
    }
    
    
    iftVoxel auxVoxel = maxVoxel_1;
    auxVoxel.x = auxVoxel.x + 120;
    auxVoxel.y = auxVoxel.y + 45;
    
    iftImage *roi = iftExtractROI(plateImage, maxVoxel_1, auxVoxel);
    //iftWriteImageP2(roi, filename);
    //--------------------------------------------------------------------------
    
    
    
    // Set pixel's value outside plate region to 0.
    int y;
    for(x = 0; x < orig->xsize; x++ ){
        for(y = 0; y < orig->ysize; y++){
            v.x = x;
            v.y = y;
            
            int coord = iftGetVoxelIndex(orig, v);
            orig->val[coord] = 0;
            
        }
    }
    
    
    // Set pixel's value inside plate region to 4095.
    for(xCoord = maxVoxel_1.x; xCoord < (maxVoxel_1.x + tileXSize); xCoord++){
        for(yCoord = maxVoxel_1.y; yCoord < (maxVoxel_1.y + tileYSize); yCoord++){
            v.x = xCoord;
            v.y = yCoord;
                                
            int coord = iftGetVoxelIndex(orig, v);
            orig->val[coord] = 4095;
        }
    }
    
    return roi;     
}



iftImage *selectCandidates(iftImage *orig, char *filename, int binarizationMethod, int pooling) {
   
    iftAdjRel *A = NULL;  
    iftKernel *Kx = NULL;
    
    iftImage *aux[4];
        
    // Performs image binarization using Sauvola algorithm.
    if(binarizationMethod == 0){
        aux[0] = binarizationBySauvola(orig);
    } else if(binarizationMethod == 1){
        aux[0] = binarizationByNiblack(orig);
    } else {
        int t = iftOtsu(orig);
        int max = iftMaximumValue(orig);

        aux[0] = iftThreshold(orig, t, max, 4095);
    }    
    
    
    // Performs Sobel to get responses in the x-axis.
    Kx = iftSobelXKernel2D();
    aux[1] = iftFastLinearFilter(aux[0], Kx, 0);
    aux[2] = iftAbs(aux[1]);    
    
    iftDestroyImage(&aux[0]);
    iftDestroyImage(&aux[1]);
    
    // Performs image open operator.    
    A = iftCircular(1.0);
    aux[3] = iftOpen(aux[2], A);
    iftDestroyImage(&aux[2]);
    
    if(pooling == 0){
        // Performs pooling.
        aux[4] = iftAlphaPooling(aux[3], A, 4, 2);  

        // Finds region with high number of white pixels.
        aux[0] = computeDenserRegions(aux[4], orig, filename);      
    } else{
        // Finds region with high number of white pixels.
        aux[0] = computeDenserRegions(aux[3], orig, filename);
    }

    iftDestroyImage(&aux[3]); 
    iftDestroyAdjRel(&A);
    iftDestroyKernel(&Kx);    
    
    return aux[0];   
}

#endif //IFT_IFTSELECTCANDIDATES_H
