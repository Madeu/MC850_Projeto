#include "ift.h"
#include "iftSeedsGenerator.h"
#include "iftSegmentation.h"

iftMatrix *iftCameraCalibrationMatrix(iftPoint *src, iftPoint *dst) {
    iftMatrix *A = iftCreateMatrix(8,8), *c = iftCreateMatrix(1,8);
    iftMatrix *invA, *b, *B;

    A->val[iftGetMatrixIndex(A,0,0)] = src[0].x;
    A->val[iftGetMatrixIndex(A,1,0)] = src[0].y;
    A->val[iftGetMatrixIndex(A,2,0)] = 1.0;
    A->val[iftGetMatrixIndex(A,6,0)] = -src[0].x*dst[0].x;
    A->val[iftGetMatrixIndex(A,7,0)] = -src[0].y*dst[0].x;

    A->val[iftGetMatrixIndex(A,3,1)] = src[0].x;
    A->val[iftGetMatrixIndex(A,4,1)] = src[0].y;
    A->val[iftGetMatrixIndex(A,5,1)] = 1.0;
    A->val[iftGetMatrixIndex(A,6,1)] = -src[0].x*dst[0].y;
    A->val[iftGetMatrixIndex(A,7,1)] = -src[0].y*dst[0].y;


    A->val[iftGetMatrixIndex(A,0,2)] = src[1].x;
    A->val[iftGetMatrixIndex(A,1,2)] = src[1].y;
    A->val[iftGetMatrixIndex(A,2,2)] = 1.0;
    A->val[iftGetMatrixIndex(A,6,2)] = -src[1].x*dst[1].x;
    A->val[iftGetMatrixIndex(A,7,2)] = -src[1].y*dst[1].x;

    A->val[iftGetMatrixIndex(A,3,3)] = src[1].x;
    A->val[iftGetMatrixIndex(A,4,3)] = src[1].y;
    A->val[iftGetMatrixIndex(A,5,3)] = 1.0;
    A->val[iftGetMatrixIndex(A,6,3)] = -src[1].x*dst[1].y;
    A->val[iftGetMatrixIndex(A,7,3)] = -src[1].y*dst[1].y;

    A->val[iftGetMatrixIndex(A,0,4)] = src[2].x;
    A->val[iftGetMatrixIndex(A,1,4)] = src[2].y;
    A->val[iftGetMatrixIndex(A,2,4)] = 1.0;
    A->val[iftGetMatrixIndex(A,6,4)] = -src[2].x*dst[2].x;
    A->val[iftGetMatrixIndex(A,7,4)] = -src[2].y*dst[2].x;

    A->val[iftGetMatrixIndex(A,3,5)] = src[2].x;
    A->val[iftGetMatrixIndex(A,4,5)] = src[2].y;
    A->val[iftGetMatrixIndex(A,5,5)] = 1.0;
    A->val[iftGetMatrixIndex(A,6,5)] = -src[2].x*dst[2].y;
    A->val[iftGetMatrixIndex(A,7,5)] = -src[2].y*dst[2].y;

    A->val[iftGetMatrixIndex(A,0,6)] = src[3].x;
    A->val[iftGetMatrixIndex(A,1,6)] = src[3].y;
    A->val[iftGetMatrixIndex(A,2,6)] = 1.0;
    A->val[iftGetMatrixIndex(A,6,6)] = -src[3].x*dst[3].x;
    A->val[iftGetMatrixIndex(A,7,6)] = -src[3].y*dst[3].x;

    A->val[iftGetMatrixIndex(A,3,7)] = src[3].x;
    A->val[iftGetMatrixIndex(A,4,7)] = src[3].y;
    A->val[iftGetMatrixIndex(A,5,7)] = 1.0;
    A->val[iftGetMatrixIndex(A,6,7)] = -src[3].x*dst[3].y;
    A->val[iftGetMatrixIndex(A,7,7)] = -src[3].y*dst[3].y;

    c->val[iftGetMatrixIndex(c,0,0)] = dst[0].x; 
    c->val[iftGetMatrixIndex(c,0,1)] = dst[0].y; 
    c->val[iftGetMatrixIndex(c,0,2)] = dst[1].x; 
    c->val[iftGetMatrixIndex(c,0,3)] = dst[1].y; 
    c->val[iftGetMatrixIndex(c,0,4)] = dst[2].x; 
    c->val[iftGetMatrixIndex(c,0,5)] = dst[2].y; 
    c->val[iftGetMatrixIndex(c,0,6)] = dst[3].x; 
    c->val[iftGetMatrixIndex(c,0,7)] = dst[3].y; 

    invA = iftInvertMatrix(A);
    b    = iftMultMatrices(invA,c);
    B    = iftCreateMatrix(3,3); 

    B->val[iftGetMatrixIndex(B,0,0)] = b->val[iftGetMatrixIndex(b,0,0)];
    B->val[iftGetMatrixIndex(B,1,0)] = b->val[iftGetMatrixIndex(b,0,1)];
    B->val[iftGetMatrixIndex(B,2,0)] = b->val[iftGetMatrixIndex(b,0,2)];
    B->val[iftGetMatrixIndex(B,0,1)] = b->val[iftGetMatrixIndex(b,0,3)];
    B->val[iftGetMatrixIndex(B,1,1)] = b->val[iftGetMatrixIndex(b,0,4)];
    B->val[iftGetMatrixIndex(B,2,1)] = b->val[iftGetMatrixIndex(b,0,5)];
    B->val[iftGetMatrixIndex(B,0,2)] = b->val[iftGetMatrixIndex(b,0,6)];
    B->val[iftGetMatrixIndex(B,1,2)] = b->val[iftGetMatrixIndex(b,0,7)];
    B->val[iftGetMatrixIndex(B,2,2)] = 1.0;

    iftDestroyMatrix(&A);
    iftDestroyMatrix(&c);
    iftDestroyMatrix(&b);
    iftDestroyMatrix(&invA);

    return(B);
}

iftImage *iftPlateWatershed(iftImage *orig, iftImage *candidate, iftImage *no_candidate) {
    iftLabeledSet *seed=NULL;

    for (int p=0; p < candidate->n; p++) {
      if (candidate->val[p]!=0)
          iftInsertLabeledSet(&seed,p,255); 
      if (no_candidate->val[p]==0)
          iftInsertLabeledSet(&seed,p,0); 
    }
    iftAdjRel *B     = iftCircular(1.5);
    iftImage *basins = iftSobelGradientMagnitude(orig);
    iftImage *label   = iftWatershed(basins, B, seed);

    iftDestroyImage(&basins);
    iftDestroyLabeledSet(&seed);

    return(label);
}

iftImage *iftSharpPlateCorners(iftImage *plate) {
    iftAdjRel *A1   = iftRectangular(9,9);
    iftAdjRel *A2   = iftRectangular(11,11);
    iftImage  *ero  = iftErode(plate,A1);
    iftImage  *dil  = iftDilate(ero,A2);

    iftDestroyImage(&ero);
    iftDestroyAdjRel(&A1);
    iftDestroyAdjRel(&A2);
    return(dil);
}


iftPoint *iftTheFourPlateCorners(iftImage *plate) {
    iftAdjRel *A = iftCircular(1.0);
    iftSet *S = iftObjectBorderSet(plate, A), *Saux1, *Saux2;
    iftPoint *P = (iftPoint *) calloc(4,sizeof(iftPoint));

    iftDestroyAdjRel(&A);

    int dmax = INFINITY_INT_NEG;

    Saux1 = S;
    while (Saux1 != NULL) {
        int p1 = Saux1->elem;
        iftVoxel u1 = iftGetVoxelCoord(plate,p1);
        Saux2 = Saux1;
        while (Saux2 != NULL) {
            int p2 = Saux2->elem;
            iftVoxel u2 = iftGetVoxelCoord(plate,p2);
            int dist = iftSquaredVoxelDistance(u1,u2);
            if (dist > dmax) {
                P[0].x = u1.x;  
                P[0].y = u1.y;
                P[1].x = u2.x;  
                P[1].y = u2.y;
                dmax   = dist;
            } 
            Saux2 = Saux2->next;
        }
        Saux1 = Saux1->next;
    }


    /* Compute the normal vector to P[0]P[1] */

    iftVector N, U; 

    U.z = N.z = 0;
    U.x = (P[1].x - P[0].x); 
    U.y = (P[1].y - P[0].y); 
    U   = iftNormalizeVector(U);
    N.x = -U.y; N.y = U.x;

    /* Find the two other corners at the positive and negative sides of
    the line P[0]P[1] */

    float Dmax=INFINITY_FLT_NEG, Dmin=INFINITY_FLT; 

    Saux1 = S;
    while (Saux1 != NULL) {
        int p1 = Saux1->elem;
        iftVoxel  u1 = iftGetVoxelCoord(plate,p1);
        iftVector V1; 

        V1.z = 0;
        V1.x = u1.x - P[0].x;
        V1.y = u1.y - P[0].y;

        float dist = iftInnerProduct(V1,N);

        if (dist < Dmin) {
            Dmin = dist;
            P[2].x = u1.x; 
            P[2].y = u1.y; 
        }
        if (dist > Dmax) {
            Dmax = dist;
            P[3].x = u1.x; 
            P[3].y = u1.y; 
        }
        Saux1 = Saux1->next;
    }

    /* Sort the points in clockwise order */

    iftPoint Pmean; 
    iftPoint *Q = (iftPoint *) calloc(4,sizeof(iftPoint));

    Pmean.x = (P[0].x + P[1].x + P[2].x + P[3].x)/4.0;
    Pmean.y = (P[0].y + P[1].y + P[2].y + P[3].y)/4.0;
    Pmean.z = 0;

    for (int i=0; i < 4; i++) {
        if ((P[i].x < Pmean.x)&&(P[i].y < Pmean.y)){
            Q[0].x = P[i].x;
            Q[0].y = P[i].y;
        }
        if ((P[i].x > Pmean.x)&&(P[i].y < Pmean.y)){
            Q[1].x = P[i].x;
            Q[1].y = P[i].y;
        }
        if ((P[i].x > Pmean.x)&&(P[i].y > Pmean.y)){
            Q[2].x = P[i].x;
            Q[2].y = P[i].y;
        }
        if ((P[i].x < Pmean.x)&&(P[i].y > Pmean.y)){
            Q[3].x = P[i].x;
            Q[3].y = P[i].y;
        }
    }

    free(P);
    iftDestroySet(&S);
    return(Q);
}

iftImage *iftCorrectImage(iftImage *img, iftPoint *src_pt, iftPoint *dst_pt, int xsize, int ysize) {
    iftImage  *cimg = iftCreateImage(xsize,ysize,1);
    iftMatrix *T,*InvT, *src = iftCreateMatrix(1,3), *dst=iftCreateMatrix(1,3);
    iftPoint P;
    iftVoxel v,u;
    int q;

    T = iftCameraCalibrationMatrix(src_pt, dst_pt); 
    //iftPrintMatrix(T); 

    InvT = iftInvertMatrix(T);

    v.z = u.z = P.z = 0;
    for (v.y = 0; v.y < cimg->ysize; v.y++) 
    for (v.x = 0; v.x < cimg->xsize; v.x++){
        q    = iftGetVoxelIndex(cimg,v);
        dst->val[iftGetMatrixIndex(dst,0,0)] = v.x;
        dst->val[iftGetMatrixIndex(dst,0,1)] = v.y;
        dst->val[iftGetMatrixIndex(dst,0,2)] = 1;
        src   = iftMultMatrices(InvT,dst);
        P.x   = src->val[iftGetMatrixIndex(src,0,0)]/src->val[iftGetMatrixIndex(src,0,2)];
        P.y   = src->val[iftGetMatrixIndex(src,0,1)]/src->val[iftGetMatrixIndex(src,0,2)];
        u.x   = ROUND(P.x);
        u.y   = ROUND(P.y);

        if (iftValidVoxel(img,u)){
            cimg->val[q] = iftImageValueAtPoint2D(img,P);
        }
    }

    iftDestroyMatrix(&InvT);
    iftDestroyMatrix(&src);
    iftDestroyMatrix(&dst);
    iftDestroyMatrix(&T);

    return(cimg);
}

iftImage *getCorrectedImage(iftImage *orig, iftImage *candidate, iftVoxel cand_pos) {
    iftVoxel center = getBigCenter(candidate, 1000, 2);

    center.x += cand_pos.x;
    center.y += cand_pos.y;

    iftImage* labelTrue = drawLabelGen(orig, center, 8, 5);
    iftImage* labelFalse = drawLabelGen(orig, center, 80, 30);

    iftImage *label = iftPlateWatershed(orig, labelTrue, labelFalse);
    iftImage *watershed_img = iftSharpPlateCorners(label);
    iftDestroyImage(&label);
    iftDestroyImage(&labelTrue);
    iftDestroyImage(&labelFalse);

    iftPoint *src = iftTheFourPlateCorners(watershed_img);
    iftPoint *dst = (iftPoint *) calloc(4,sizeof(iftPoint));
    iftDestroyImage(&watershed_img);

    dst[0].x = 0;
    dst[0].y = 0;
    dst[1].x = 199;
    dst[1].y = 0;
    dst[2].x = 199;
    dst[2].y = 59;
    dst[3].x = 0;
    dst[3].y = 59;

    iftImage *cimg = iftCorrectImage(orig,src,dst,200,60);
    return cimg;
}
