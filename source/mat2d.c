/*
Filename: mat2d.c
Author:   AllAcacia
*/


#include "mat2d.h"


Matrix2D_Float* mat2Dfloat_init(uint8_t m, uint8_t n)
{
    // allocates memory to new matrix object
    Matrix2D_Float* ptr = malloc(sizeof(Matrix2D_Float));
    ptr->mat_float = malloc(m * n * sizeof(float));
    ptr->m = m;
    ptr->n = n;
    return ptr;
}


void mat2Dfloat_del(Matrix2D_Float* ptr)
{
    // frees memory of matrix object
    free(ptr->mat_float);
    free(ptr);
}


float mat2Dfloat_return(const Matrix2D_Float* ptr, size_t i, size_t j)
{
    float result = 0;
    if ((i < ptr->m) && (j < ptr->n)){
        result = *((ptr->mat_float) + (ptr->n)*i + j);
    }
    return result;
}


void mat2Dfloat_insert(Matrix2D_Float* ptr, size_t i, size_t j, float val)
{
    if ((i < ptr->m) && (j < ptr->n)){
        *((ptr->mat_float) + (ptr->n)*i + j) = val;
    }
}


void mat2Dfloat_matmul(const Matrix2D_Float* a, const Matrix2D_Float* b, Matrix2D_Float* c)
{
    // carries out matrix multiplication (a@b) provided
    // both of their dimensions conform, and c is correct.
    if(a && b && c) {
        if ((c->m == a->m) && (c->n == b->n) && (a->n == b->m)) {
            for(size_t i = 0; i < a->m; i++) { // for each row
                for(size_t j = 0; j < b->n; j++) { // for each col
                    float sum = 0;
                    for(size_t k = 0; k < a->n; k++) {
                        sum += mat2Dfloat_return(a, i, k) * mat2Dfloat_return(b, k, j); // get index 
                    }
                    mat2Dfloat_insert(c, i, j, sum); // insert result
                }
            }
        }
    }
}