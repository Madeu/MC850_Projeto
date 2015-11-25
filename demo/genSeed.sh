#!/bin/bash

make findChar

FILES=$1/*.pgm
DEST=$2

for f in $FILES; do
  fname=$(basename $f)
  fdest=$DEST/$fname
  fdesta="$DEST"/alig-"$fname"

  ../bin/findChar $f $fdest $fdesta

done
