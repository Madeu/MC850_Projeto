#!/bin/bash

FILES=$1/*.pgm
TAMX=$2
TAMY=$3
for f in $FILES; do
  echo $f "resize" "$TAMX"x"$TAMY"
  convert $f -resize "$TAMX"x"$TAMY"! $f

done