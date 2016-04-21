#!/bin/bash
# This script will run the filter
echo "Type the number of  processors (1 to 10), followed by [ENTER]:"

read np
if [  "$np"  -ge  1 ] &&  [ "$np"  -le  10 ]
then
    echo "correct number of processors"
    echo "Type the image to apply the filter (im1.ppm | im2.ppm |im3.ppm), followed by [ENTER]:"
    read origenFile
    if [  "$origenFile"  == "im1.ppm" ] || [ "$origenFile"  == "im2.ppm"  ] || [  "$origenFile"  == "im3.ppm" ]
    then
        echo "correct name"
        mpirun -np "$np" ./thresc ../../originalImag/"$origenFile" ../../newImag/"$origenFile"
    else
        echo "incorrect name"
    fi
else
    echo "incorrect number of processors"
fi
