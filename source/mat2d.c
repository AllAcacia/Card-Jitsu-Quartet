/*
Filename: mat2d.c
Author:   AllAcacia
*/


#include "mat2d.h"


Matrix2D_Float* mat2Dfloat_init(const size_t m, const size_t n)
{
    // allocates memory to new matrix object
    Matrix2D_Float* mat = malloc(sizeof(Matrix2D_Float));
    mat->mat_float = calloc(n*m, sizeof(float));
    mat->m = m;
    mat->n = n;
    if(mat->mat_float != NULL) {
        mat2Dfloat_fillzeros(mat);
    }
    return mat;
}


void mat2Dfloat_fillzeros(const Matrix2D_Float* ptr)
{
    // fills matrix with zeros.
    for(size_t i = 0; i < ptr->m; i++) { // for each row
        for(size_t j = 0; j < ptr->n; j++) { // for each col
            mat2Dfloat_insert(ptr, i, j, 0.0f); // insert 0.0
        }
    }
}


void mat2Dfloat_del(Matrix2D_Float* ptr)
{
    // frees memory of matrix object
    free(ptr->mat_float);
    free(ptr);
}


float mat2Dfloat_return(const Matrix2D_Float* ptr, const size_t i, const size_t j)
{
    float result = 0;
    if((i < ptr->m) && (j < ptr->n)){
        result = *((ptr->mat_float) + (ptr->n)*i + j);
    }
    return result;
}


void mat2Dfloat_insert(const Matrix2D_Float* ptr, const size_t i, const size_t j, float val)
{
    if((i < ptr->m) && (j < ptr->n)){
        *((ptr->mat_float) + (ptr->n)*i + j) = val;
    }
}


void mat2Dfloat_arithmetic(const Matrix2D_Float* a, const Matrix2D_Float* b, Matrix2D_Float* c, const MathOp mode)
{
    // does matrix, element-wise arithmetic of one of four types.
    // This function stores the result in a matrix passed in parameters.
    if(a && b) {
        if((a->m == b->m) && (a->n == b->n)) {
            for (size_t i = 0; i < a->m; i++) { // for each row
                for (size_t j = 0; j < a->n; j++) { // for each col
                    float av = mat2Dfloat_return(a, i, j);
                    float bv = mat2Dfloat_return(b, i, j);
                    
                    if(mode == OP_ADD) {        // addition
                        mat2Dfloat_insert(c, i, j, av + bv);
                    } else if(mode == OP_SUB) { // subtraction
                        mat2Dfloat_insert(c, i, j, av - bv);
                    } else if(mode == OP_MUL) { // multiplication
                        mat2Dfloat_insert(c, i, j, av * bv);
                    } else if(mode == OP_DIV && bv != 0.0f) { // division
                        mat2Dfloat_insert(c, i, j, av / bv);
                    }
                }
            }
        }
    }
    // return c;
}


void mat2Dfloat_scale(const Matrix2D_Float* a, Matrix2D_Float* b, const float var)
{
    // linearly scales matrix "a", outputted into "b".
    if(a && b) {
        if((a->m == b->m) && (a->n == b->n)) {
            for (size_t i = 0; i < a->m; i++) { // for each row
                for (size_t j = 0; j < a->n; j++) { // for each col
                    float av = mat2Dfloat_return(a, i, j);
                    mat2Dfloat_insert(b, i, j, av * var);
                }
            }
        }
    }
}


void mat2Dfloat_matmul(const Matrix2D_Float* a, const Matrix2D_Float* b, Matrix2D_Float* c)
{
    // carries out matrix multiplication (a@b) provided
    // both of their dimensions conform, and c is correct.
    // This function stores the result in a matrix passed in parameters.
    if(a && b) {
        if((a->n == b->m) && (c->m == a->m) && (c->n == b->n)) {
            for(size_t i = 0; i < a->m; i++) { // for each row
                for(size_t j = 0; j < b->n; j++) { // for each col
                    float sum = 0;
                    for(size_t k = 0; k < a->n; k++) {
                        sum += mat2Dfloat_return(a, i, k) * mat2Dfloat_return(b, k, j);
                    }
                    mat2Dfloat_insert(c, i, j, sum); // insert result
                }
            }
        }
    }
    // return c;
}