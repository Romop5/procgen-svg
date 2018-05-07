#!/bin/bash

# This script creates SVG pictures for different iteration number
# then converts these pictures to jpg
# and then use ffmpeg to create moveable gif


if  [[ $# -lt 2 ]] 
    then 

    echo "USAGE: ./$0 <scriptName> <iterationCount> "
    exit 1;
fi;


SCRIPTNAME=$1

rm -rf gifPictures
mkdir gifPictures
# 1. Take pictures

for i in `seq 1 $2`; do 
 
OUTPUT_SVG_NAME=`printf "gifPictures/%05d.svg" $i`
./svgcreator ${SCRIPTNAME} ${OUTPUT_SVG_NAME} "{\"iterations\": $i}"
done;
# 2. Convert to gif

convert -delay 10 -loop 0 gifPictures/*svg ${SCRIPTNAME}.gif

echo "It's done in ${SCRIPTNAME}.gif"



