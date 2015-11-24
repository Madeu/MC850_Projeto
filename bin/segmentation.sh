#!/bin/bash

mkdir ../sgm
mkdir ../seeds
mkdir ../characters
mkdir ../plates

echo "Computing seeds..."
./plateSegmentation $2  &> /dev/null

echo "Preparing files to segment plate..."
cp $1/*.pgm ../sgm/
cp gradient ../sgm/
cp watershed ../sgm/
cp getPlate ../sgm/
cp iftCorrectPlate ../sgm/
cp iftCharacterSeparation ../sgm/

cd ../sgm/

echo "Segmenting plates and extracting characters..."
for f in ./*.pgm; do
  fnameExt=$(basename $f)
  fnameNoExt=$(basename $f .pgm)
  echo "$fnameExt"
  ./gradient $fnameExt ../seeds/"$fnameNoExt"_seeds.txt 0  &> /dev/null
  ./watershed $fnameExt "$fnameNoExt"_grad.pgm ../seeds/"$fnameNoExt"_seeds.txt  &> /dev/null
  ./getPlate "$fnameNoExt"_seg_partial.pgm  &> /dev/null
  ./iftCorrectPlate $fnameExt "$fnameNoExt"_seg_partialBW.pgm
  ./iftCharacterSeparation "$fnameNoExt"_fixed_orientation_plate.pgm
done

# compute segmentation error
# ./computeError plate_label_dir_path plate_comp_dir_path(*_seg_partialBW.pgm files) 

echo "Process finished."
mv *_character.pgm ../characters/
mv *_plate.pgm ../plates/
#mv *_fixed_orientation_plate.pgm ../plates/


