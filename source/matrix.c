/*
Filename: matrix.c
Author:   AllAcacia
*/


#include "matrix.h"


Matrix2D_Float* mat2Dfloat_init(uint8_t m, uint8_t n)
{
    Matrix2D_Float* ptr = malloc(sizeof(Matrix2D_Float));
    ptr->mat_float = malloc(m * n * sizeof(float));
    ptr->m = m;
    ptr->n = n;
    return ptr;
}