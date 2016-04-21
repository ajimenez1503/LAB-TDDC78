/*
  File: blurfilter.h
  Declaration of pixel structure and blurfilter function.
 */
#ifndef _BLURFILTER_H_
#define _BLURFILTER_H_
#include <stdio.h>
#include "ppmio.h"
#include <stdlib.h>
#include <mpi.h>

/* NOTE: This structure must not be padded! */
typedef struct _pixel {
    unsigned char r,g,b;
} pixel;

void blurfilter(int xsize, int ysize, pixel* src, int radius, double *w);

#endif
