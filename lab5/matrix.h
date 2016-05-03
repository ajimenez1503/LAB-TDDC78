#include "coordinate.h"
#include <stdio.h>
#include<stdlib.h>
#ifndef _matrix_h
#define _matrix_h
struct Matrix
{
    unsigned int nColumns;
    unsigned int nRows;
    pcord_t** matrix;
};

struct Matrix CreateNewMatrix(unsigned int c,unsigned int r);
pcord_t * GetMatrixValue(struct Matrix* m,unsigned int x,unsigned int y);//x column and y row
void SetMatrixValue(struct Matrix* m,unsigned int x,unsigned int y,pcord_t new);//x column and y row
void freeMatrix(struct Matrix* m);


#endif//_matrix_h
