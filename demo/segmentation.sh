#!/bin/bash

./plateSegmentation ../candidates

mkdir sgm
cp ../imgs/*.pgm sgm/
cp gradient ../sgm/
cp watershed ../sgm/

cd ../sgm/

for f in *.pgm; do
  fnameExt=$(basename $f)
  fnameNoExt=$(basename $f .pgm)
  ./gradient $fnameExt ../seeds/"$fnameNoExt"_seeds.txt 0
  ./watershed $fnameExt "$fnameNoExt"_grad.pgm ../seeds/"$fnameNoExt"_seeds.txt

done


