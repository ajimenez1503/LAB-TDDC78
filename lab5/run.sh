#!/bin/bash
# This script will run the filter
echo "Type the number of  processors (1 to 10), followed by [ENTER]:"

read np
if [  "$np"  -ge  1 ] &&  [ "$np"  -le  10 ]
then
    echo "correct number of processors"
    mpirun -np "$np" ./simulator 
else
    echo "incorrect number of processors"
fi
