#!/bin/bash

make recognizePlate

FILES=$1/*.pgm
DEST=$2

mkdir -p ../caracteres

for f in $FILES; do
  fname=$(basename $f)
  echo $fname
  fdest=$DEST/$fname

  ../bin/recognizePlate $f $fdest $fdesta ../svm/svm_ova

  ../bin/charExtraction $fdest $fname

  mv *.pgm ../caracteres

done
