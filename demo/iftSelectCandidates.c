/* 
 * MO815 - Análise de Imagem Orientada a um Problema do Mundo Real
 *
 * Detecção de placas
 *
 * Autores: Amadeu Bonfante
 *	  : Luis Claudio Sugi Afonso
 *	  : Luiz Antonio Falaguasta Barbosa
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "ift.h"
#include "iftSelectCandidates.h"

/*
 * 
 */
int main(int argc, char** argv) {
    
    iftImage* orig;
    timer *t1 = NULL, *t2 = NULL;
    char outfile[100];

    /*--------------------------------------------------------*/

    int MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();

    /*--------------------------------------------------------*/



    if (argc != 5) {
        fprintf(stdout, "\nUsage: iftSelectCandidates <input images folder> <output candidates folder> "
                "<binarization method> <pooling>\n");
        fprintf(stdout, "       input images folder:       path to original grayscale images\n");
        fprintf(stdout, "       output candidate folder: path to binary images with candidates for plate location\n");
        fprintf(stdout, "       binarization method: 0 - Sauvola\n");
        fprintf(stdout, "       		     1 - Niblack\n");
        fprintf(stdout, "       		     2 - Otsu\n");
        fprintf(stdout, "       pooling: 0 - No\n");
        fprintf(stdout, "       	 1 - Yes\n");
        exit(1);
    } 

    int binarization = atoi(argv[3]);
    int pooling = atoi(argv[4]);

    if((binarization != 0 && binarization != 1 && binarization != 2) || (pooling != 0 && pooling != 1)) {
        fprintf(stdout, "\nUsage: iftSelectCandidates <input images folder> <output candidates folder> "
                "<binarization method> <pooling>\n");
        fprintf(stdout, "       input images folder:       path to original grayscale images\n");
        fprintf(stdout, "       output candidate folder: path to binary images with candidates for plate location\n");
        fprintf(stdout, "       binarization method: 0 - Sauvola\n");
        fprintf(stdout, "       		     1 - Niblack\n");
        fprintf(stdout, "       		     2 - Otsu\n");
        fprintf(stdout, "       pooling: 0 - No\n");
        fprintf(stdout, "       	 1 - Yes\n");
        exit(1);
    } 

    printf("\n Performing plate detection using ");
        
    if(binarization == 0){
	printf("Sauvola... ");
    } else if(binarization == 1){
	printf("Niblack... ");
    } else {
	printf("Otsu... ");
    }   

    if(pooling == 1){
	printf("with Pooling...\n\n ");     
    } else{
	printf("without Pooling...\n\n ");  
    }

    iftDir* inputDir = iftLoadFilesFromDirectory(argv[1], "pgm");

    int i;
    for (i = 0; i < inputDir->nfiles; ++i) {

        orig = iftReadImageByExt(inputDir->files[i]->pathname);

        t1 = iftTic();

        iftImage *candidates = selectCandidates(orig, argv[2], iftBasename(inputDir->files[i]->pathname), binarization, pooling);     

        t2 = iftToc();

        sprintf(outfile, "%s/%s", argv[2], iftBasename(inputDir->files[i]->pathname));

        iftWriteImageP2(candidates, outfile);

        fprintf(stdout, "%dth image plate candidates located in %f ms\n\n", i+1,  iftCompTime(t1, t2));

        iftDestroyImage(&orig);
        iftDestroyImage(&candidates);
    }

    /* ---------------------------------------------------------- */
    MemDinFinal = iftMemoryUsed();
    if (MemDinInicial != MemDinFinal)
        printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
               MemDinInicial, MemDinFinal);

    return (0);
}


