/*
  File: ppmio.c

  Implementation of PPM image file IO functions.
 */
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ppmio.h"

int read_ppm (const char * fname,int * xpix, int * ypix, int * max, char * data) {

  char ftype[40];
  char ctype[40] = "P6";
  char line[80];
  int errno;

  FILE * fp;
  errno = 0;

  if (fname == NULL) fname = "\0";
  fp = fopen (fname, "r");//open to read
  if (fp == NULL) {
    fprintf (stderr, "read_ppm failed to open %s: %s\n", fname,
	     strerror (errno));
    return 1;
  }

  fgets(line, 80, fp);
  sscanf(line, "%s", ftype);//save the type

  while (fgets(line, 80, fp) && (line[0] == '#'));//read all the lines with # (header)

  sscanf(line, "%d%d", xpix, ypix);//save size x && y
  fscanf(fp, "%d\n", max);

  if(*xpix * *ypix > MAX_PIXELS) {
     fprintf (stderr, "Image size is too big\n");
    return 4;
 };

  if (strncmp(ftype, ctype, 2) == 0) {//compare type, type should be p6
    if (fread (data, sizeof (char), *xpix * *ypix * 3, fp) !=*xpix * *ypix * 3) {//read all the data of the file on data //*3 because r g b
      perror ("Read failed");
      return 2;
    }
  } else {
    fprintf (stderr, "Wrong file format: %s\n", ftype);
  }

  if (fclose (fp) == EOF) {
    perror ("Close failed");
    return 3;
  }

  return 0;

}


int write_ppm (const char * fname, int xpix, int ypix, char * data) {

  FILE * fp;
  int errno = 0;

  if (fname == NULL) fname = "\0";
  fp = fopen (fname, "w");
  if (fp == NULL) {
    fprintf (stderr, "write_ppm failed to open %s: %s\n", fname,
	     strerror (errno));
    return 1;
  }

  fprintf (fp, "P6\n");//write the type
  fprintf (fp, "%d %d 255\n", xpix, ypix);//write the size z & y
  if (fwrite (data, sizeof (char), xpix*ypix*3, fp) != xpix*ypix*3) {//write all the data  //*3 because r g b
    perror ("Write failed");
    return 2;
  }
  if (fclose (fp) == EOF) {
    perror ("Close failed");
    return 3;
  }
  return 0;
}
