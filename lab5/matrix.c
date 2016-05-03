#include "matrix.h"

struct Matrix CreateNewMatrix(unsigned int c,unsigned int r){
    struct Matrix mat;
    mat.nColumns = c;
    mat.nRows =r;
    mat.matrix = (pcord_t**)calloc(c, sizeof(pcord_t));

    unsigned int i;
    for(i = 0; i < c; i++)
    {
        mat.matrix[i] = (pcord_t *)calloc(r,sizeof(pcord_t));
    }
    return mat;
}

pcord_t * GetMatrixValue(struct Matrix* m,unsigned int x,unsigned int y){
    if(x>=0 && x<m->nRows && y>=0 && y<m->nColumns){
        return &m->matrix[x][y];
    }
    else{
        printf("Error: bad access,(%d,%d) \n",x,y );
        exit(1);//bad access
    }
}
void SetMatrixValue(struct Matrix* m,unsigned int x,unsigned int y,pcord_t new){
    if(x>=0 && x<m->nRows && y>=0 && y<m->nColumns){
            m->matrix[x][y]=new;
    }
    else{
        printf("Error: bad access,(%d,%d) \n",x,y );
        exit(1);//bad access
    }
}
void freeMatrix(struct Matrix* m){
    int i=0;
    for(i = 0; i < m->nColumns; i++){
        free(m->matrix[i]);
    }
    free(m->matrix);
}
