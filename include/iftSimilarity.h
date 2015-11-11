#ifndef IFT_SIMILARITY_H_
#define IFT_SIMILARITY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "iftCommon.h"
#include "iftAdjacency.h"
#include "iftImage.h"
#include "iftFImage.h"
#include "iftRepresentation.h"
#include "iftObjectModels.h"
#include "iftSegmentation.h"

  typedef float 	(*iftImageSimilarityFunction) (iftImage *baseImage, iftImage *auxImage); 

  float 		iftSimilarityByDice(iftImage *baseImage, iftImage *auxImage);
  float 		iftSimilarityByASSD(iftImage *baseImage, iftImage *auxImage);
  iftMatrix 	*iftSimilarityMatrix(fileList *imageFiles, iftImageSimilarityFunction similarityFunction, float main_diagonal);
  void 		iftWriteSimilarityMatrix(iftMatrix *result, fileList *imageFiles, char *out, int maximize);
  double          iftShannonEntropy(iftImage *image);
  double          iftJointEntropy(iftImage *image1, iftImage *image2);
  double          iftNormalizedMutualInformation(iftImage *image1, iftImage *image2);
  float           iftMeanDistFromSourceToTarget(iftImage *bin_source, iftImage * bin_target);
  float           iftMeanDistBetweenBoundaries(iftImage *bin1, iftImage * bin2);

#ifdef __cplusplus
}
#endif

#endif


