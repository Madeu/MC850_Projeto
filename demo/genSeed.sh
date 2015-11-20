#!/bin/bash

make genSeed

FILES=$1/*.pgm
DEST=$2

for f in $FILES; do
  fname=$(basename $f)
  fdest=$DEST/$fname

  ../bin/genSeed $f $fdest

done
