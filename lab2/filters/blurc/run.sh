#!/bin/bash
# This script will run the filter
echo "correct number of processors"
echo "Type the radius (1 to 1000), followed by [ENTER]:"
read radious
if [  "$radious"  -ge  1 ] &&  [ "$radious"  -le  1000 ]
then
    echo "correct radious"
    echo "Type the image to apply the filter (im1.ppm | im2.ppm |im3.ppm|im4.ppm), followed by [ENTER]:"
    read origenFile
    if [  "$origenFile"  == "im1.ppm" ] || [ "$origenFile"  == "im2.ppm"  ] || [  "$origenFile"  == "im3.ppm" ]|| [  "$origenFile"  == "im4.ppm" ]
    then
        echo "correct name"
        ./blurc  "$radious" ../../originalImag/"$origenFile" ../../newImag/"$origenFile"
    else
        echo "incorrect name"
    fi
else
    echo "incorrect name"
fi
