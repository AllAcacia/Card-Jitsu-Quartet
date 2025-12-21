/*
Filename: mat2d.h
Author:   AllAcacia
*/


#include <stdint.h>
#include <stdlib.h>
#include <math.h>


typedef struct {
    size_t m; // rows
    size_t n; // cols
    float* mat_float;
} Matrix2D_Float;


typedef enum {
    OP_ADD=0,
    OP_SUB,
    OP_MUL,
    OP_DIV
} MathOp;


Matrix2D_Float mat2Dfloat_init(const size_t m, const size_t n);

void mat2Dfloat_fillzeros(const Matrix2D_Float* ptr);

void mat2Dfloat_del(Matrix2D_Float* ptr);

float mat2Dfloat_return(const Matrix2D_Float* ptr, const size_t i, const size_t j);

void mat2Dfloat_insert(const Matrix2D_Float* ptr, const size_t i, const size_t j, float val);

Matrix2D_Float mat2Dfloat_arithmetic(const Matrix2D_Float* a, const Matrix2D_Float* b, const MathOp mode);

Matrix2D_Float mat2Dfloat_matmul(const Matrix2D_Float* a, const Matrix2D_Float* b);