#!/bin/bash
# This script will run the filter
echo "Type the image to apply the filter (im1.ppm | im2.ppm |im3.ppm|im4.ppm), followed by [ENTER]:"
read origenFile
if [  "$origenFile"  == "im1.ppm" ] || [ "$origenFile"  == "im2.ppm"  ] || [  "$origenFile"  == "im3.ppm" ]|| [  "$origenFile"  == "im4.ppm" ]
then
    echo "correct name"
    ./thresc ../../originalImag/"$origenFile" ../../newImag/"$origenFile"
else
    echo "incorrect name"
fi
