/*
Filename: mat2d.h
Author:   AllAcacia
*/


#include <stdint.h>
#include <stdlib.h>


typedef struct {
    size_t m; // rows
    size_t n; // cols
    float* mat_float;
} Matrix2D_Float;


Matrix2D_Float* mat2Dfloat_init(size_t m, size_t n);

void mat2Dfloat_del(Matrix2D_Float* ptr);

float mat2Dfloat_return(Matrix2D_Float* ptr, size_t i, size_t j);

void mat2Dfloat_insert(Matrix2D_Float* ptr, size_t i, size_t j, float val);

void mat2Dfloat_matmul(Matrix2D_Float* a, Matrix2D_Float* b, Matrix2D_Float* c);