#include "ift.h"


/* correction matrix * coefficient column = destination point column 
   A * b = c
   b  = Inv(A) c
   convert b into B (3x3 matrix)
   return B
*/

iftMatrix *iftObjectAlignMatrixByPCA(iftImage *bin) 
{ 
  iftDataSet *Z=iftObjectToDataSet(bin);

  iftSetStatus(Z,TRAIN);
  iftDataSet *Zc=iftCentralizeDataSet(Z);
  iftMatrix  *C = iftCovarianceMatrix(Zc);
  iftMatrix  *U,*S,*Vt;
  iftSingleValueDecomp(C,&U,&S,&Vt);
  iftDestroyMatrix(&C);
  iftDestroyMatrix(&U);
  iftDestroyMatrix(&S);
  iftDestroyDataSet(&Zc);
  iftDestroyDataSet(&Z);

  /* It assumes that the alignment must not flip the object along any
     axis. */

	int i;
  for (i=0; i < Vt->nrows; i++) 
    if (Vt->val[iftGetMatrixIndex(Vt,i,i)] < 0) 
      Vt->val[iftGetMatrixIndex(Vt,i,i)]=-Vt->val[iftGetMatrixIndex(Vt,i,i)];
  
  return(Vt);
}

iftMatrix *iftCorrectionMatrix(iftPoint *src, iftPoint *dst)
{
  iftMatrix *A = iftCreateMatrix(8,8), *c = iftCreateMatrix(1,8);
  iftMatrix *invA, *b, *B;

  A->val[iftGetMatrixIndex(A,0,0)] = src[0].x;
  A->val[iftGetMatrixIndex(A,1,0)] = src[0].y;
  A->val[iftGetMatrixIndex(A,2,0)] = 1.0;
  A->val[iftGetMatrixIndex(A,6,0)] = -src[0].x*dst[0].x;
  A->val[iftGetMatrixIndex(A,7,0)] = -src[0].y*dst[0].y;

  A->val[iftGetMatrixIndex(A,3,1)] = src[0].x;
  A->val[iftGetMatrixIndex(A,4,1)] = src[0].y;
  A->val[iftGetMatrixIndex(A,5,1)] = 1.0;
  A->val[iftGetMatrixIndex(A,6,1)] = -src[0].x*dst[0].x;
  A->val[iftGetMatrixIndex(A,7,1)] = -src[0].y*dst[0].y;


  A->val[iftGetMatrixIndex(A,0,2)] = src[1].x;
  A->val[iftGetMatrixIndex(A,1,2)] = src[1].y;
  A->val[iftGetMatrixIndex(A,2,2)] = 1.0;
  A->val[iftGetMatrixIndex(A,6,2)] = -src[1].x*dst[1].x;
  A->val[iftGetMatrixIndex(A,7,2)] = -src[1].y*dst[1].y;

  A->val[iftGetMatrixIndex(A,3,3)] = src[1].x;
  A->val[iftGetMatrixIndex(A,4,3)] = src[1].y;
  A->val[iftGetMatrixIndex(A,5,3)] = 1.0;
  A->val[iftGetMatrixIndex(A,6,3)] = -src[1].x*dst[1].x;
  A->val[iftGetMatrixIndex(A,7,3)] = -src[1].y*dst[1].y;


  A->val[iftGetMatrixIndex(A,0,4)] = src[2].x;
  A->val[iftGetMatrixIndex(A,1,4)] = src[2].y;
  A->val[iftGetMatrixIndex(A,2,4)] = 1.0;
  A->val[iftGetMatrixIndex(A,6,4)] = -src[2].x*dst[2].x;
  A->val[iftGetMatrixIndex(A,7,4)] = -src[2].y*dst[2].y;

  A->val[iftGetMatrixIndex(A,3,5)] = src[2].x;
  A->val[iftGetMatrixIndex(A,4,5)] = src[2].y;
  A->val[iftGetMatrixIndex(A,5,5)] = 1.0;
  A->val[iftGetMatrixIndex(A,6,5)] = -src[2].x*dst[2].x;
  A->val[iftGetMatrixIndex(A,7,5)] = -src[2].y*dst[2].y;


  A->val[iftGetMatrixIndex(A,0,6)] = src[3].x;
  A->val[iftGetMatrixIndex(A,1,6)] = src[3].y;
  A->val[iftGetMatrixIndex(A,2,6)] = 1.0;
  A->val[iftGetMatrixIndex(A,6,6)] = -src[3].x*dst[3].x;
  A->val[iftGetMatrixIndex(A,7,6)] = -src[3].y*dst[3].y;

  A->val[iftGetMatrixIndex(A,3,7)] = src[3].x;
  A->val[iftGetMatrixIndex(A,4,7)] = src[3].y;
  A->val[iftGetMatrixIndex(A,5,7)] = 1.0;
  A->val[iftGetMatrixIndex(A,6,7)] = -src[3].x*dst[3].x;
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


int main(int argc, char* argv []) {

  if(argc!=3) {
    iftError("Usage: iftCorrectPlate <orig.pgm> <label.pgm>", argv[0]);
  }

  iftImage   *orig=iftReadImageByExt(argv[1]);
  iftImage   *label=iftReadImageByExt(argv[2]);

  
  char mainFile[100];
  sprintf(mainFile, "%s", argv[1]);

  char* p = strtok(argv[1], ".");

  char fixedImgFile[100];
  sprintf(fixedImgFile, "%s_fixed_orientation_plate.pgm", p);

  /* Extract plate mask and find the rotation by PCA */

  iftVoxel    pos;
  iftImage   *plate=iftExtractObject(label,1,&pos);
  iftMatrix  *T = iftObjectAlignMatrixByPCA(plate);

  iftDestroyImage(&label);
  label = iftTransformImage(plate,T);

  /* Extract the same ROI from the original image */
 
  iftVoxel    end;
  end.x = pos.x + plate->xsize - 1;
  end.y = pos.y + plate->ysize - 1;
  end.z = 0;
  iftImage   *roi=iftExtractROI(orig,pos,end);

 /* Apply rotation correction */

  iftDestroyImage(&orig);
  orig  = iftTransformImage(roi,T);    
  iftDestroyMatrix(&T);
  iftDestroyImage(&roi);

 
  /* new function */

  /* iftPoint *src = (iftPoint *) calloc(4,sizeof(iftPoint)); */
  /* iftPoint *dst = (iftPoint *) calloc(4,sizeof(iftPoint)); */
  
  /* src[0].x = 150; */
  /* src[0].y = 150; */
  /* src[1].x = 237; */
  /* src[1].y = 160; */
  /* src[2].x = 243; */
  /* src[2].y = 192; */
  /* src[3].x = 152; */
  /* src[3].y = 176; */

  /* dst[0].x = 0; */
  /* dst[0].y = 0; */
  /* dst[1].x = 100; */
  /* dst[1].y = 0; */
  /* dst[2].x = 100; */
  /* dst[2].y = 25; */
  /* dst[3].x = 0; */
  /* dst[3].y = 25; */
  
  /* T = iftCorrectionMatrix(src, dst); */
  /* iftPrintMatrix(T); */

  /* exit(0); */

  /* Apply shear correction and find the final ROI */

  iftImage *origc;
  iftVoxel v;
  iftVoxel vo, vf, u;

  origc  = iftCreateImage(orig->xsize,orig->ysize,orig->zsize);

  u.z = v.z = 0;
  vo.x = vo.y = INFINITY_INT;
  vf.x = vf.y = INFINITY_INT_NEG;
  vo.z = vf.z = 0;
  for (u.y=0; u.y < label->ysize; u.y++) {
    v.x = 0;
    for (u.x=0; u.x < label->xsize; u.x++) {      
      int p = iftGetVoxelIndex(label,u);
      if (label->val[p]) {
	v.y   = u.y;
	int q = iftGetVoxelIndex(label,v);
	origc->val[q]  = orig->val[p];
	if (vo.x > v.x)
	  vo.x = v.x;
	if (vo.y > v.y)
	  vo.y = v.y;
	if (vf.x < v.x)
	  vf.x = v.x;
	if (vf.y < v.y)
	  vf.y = v.y;
	v.x++;
      }
    }
  }

  iftDestroyImage(&label);
  iftDestroyImage(&orig);

  /* Extract final ROI and scale image to a normalized size */

  iftDestroyImage(&plate);
  plate = iftExtractROI(origc,vo,vf);
  iftDestroyImage(&origc);
  float sx = 200.0/plate->xsize;
  float sy = 50.0/plate->ysize;
  origc    = iftScaleImage2D(plate,sx,sy);
  iftWriteImageP2(origc,fixedImgFile);
  iftDestroyImage(&plate);
  iftDestroyImage(&origc);


  return(0);
}
