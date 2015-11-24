#include "ift.h"

iftImage *iftHorizontalProjection(iftImage *plate)
{
  iftImage *hproj = iftCreateImage(plate->ysize,1,1);
  iftVoxel  u,v;
  int       p,q;

  v.y = v.z = 0; 
  for (p=0; p < plate->n; p++)  {
    u   = iftGetVoxelCoord(plate,p);
    v.x = u.y;
    q   = iftGetVoxelIndex(hproj,v);
    hproj->val[q] += plate->val[p];
  }

  for (p=0; p < hproj->n; p++) 
    hproj->val[p] /= plate->xsize;

  return(hproj);
}

iftImage *iftVerticalProjection(iftImage *plate)
{
  iftImage *vproj = iftCreateImage(plate->xsize,1,1);
  iftVoxel  u,v;
  int       p,q;

  v.y = v.z = 0; 
  for (p=0; p < plate->n; p++)  {
    u   = iftGetVoxelCoord(plate,p);
    v.x = u.x;
    q   = iftGetVoxelIndex(vproj,v);
    vproj->val[q] += plate->val[p];
  }

  for (p=0; p < vproj->n; p++) 
    vproj->val[p] /= plate->ysize;

  return(vproj);
}

iftImage *iftCloseHoles1D(iftImage *img)
{
  int       Imax;
  iftImage *marker=NULL,*cimg=NULL;

  Imax     = iftMaximumValue(img);
  marker   = iftCreateImage(img->xsize,1,1);
  iftSetImage(marker,Imax+1);

  marker->val[0]=img->val[0]; 
  marker->val[img->xsize-1]=img->val[img->xsize-1]; 

  cimg   = iftSupRec(img,marker);
  iftDestroyImage(&marker);

  return(cimg);
}


int main(int argc, char* argv []) {

  if(argc!=2) {
    iftError("Usage: iftCharacterSeparation <plate.pgm> ", argv[0]);
  }

  iftImage   *plate=iftReadImageByExt(argv[1]);
  iftImage   *proj=iftHorizontalProjection(plate);
  iftImage   *choles=iftCloseHoles1D(proj);
  iftImage   *residue=iftSub(choles,proj);
  iftImage   *roi=iftThreshold(residue,iftOtsu(residue),INFINITY_INT,1);

  char character[100];

  char* p = strtok(argv[1], "_");

  sprintf(character, "%s_character.pgm", p);

  /* Crop plate vertically */

  for (int p=0; p < plate->n; p++){ 
    iftVoxel u = iftGetVoxelCoord(plate,p);
    if (roi->val[u.y]==0) 
      plate->val[p] = 0;
  }

  iftDestroyImage(&proj);
  iftDestroyImage(&choles);
  iftDestroyImage(&residue);
  iftDestroyImage(&roi);

  proj=iftVerticalProjection(plate);
  choles=iftCloseHoles1D(proj);
  residue=iftSub(choles,proj);
  roi=iftThreshold(residue,iftOtsu(residue)*0.7,INFINITY_INT,1);

  /* Select characters */

  for (int p=0; p < plate->n; p++){ 
    iftVoxel u = iftGetVoxelCoord(plate,p);
    if (roi->val[u.x]==0) 
      plate->val[p] = 0;
  }

  iftWriteImageP5(plate, character);

  iftDestroyImage(&plate);
  iftDestroyImage(&proj);
  iftDestroyImage(&choles);
  iftDestroyImage(&residue);
  iftDestroyImage(&roi);
  
  return(0);
}
