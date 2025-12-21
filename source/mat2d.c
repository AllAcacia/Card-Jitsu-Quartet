/*
Filename: mat2d.c
Author:   AllAcacia
*/


#include "mat2d.h"


Matrix2D_Float* mat2Dfloat_init(size_t m, size_t n)
{
    // allocates memory to new matrix object
    Matrix2D_Float* ptr = malloc(sizeof(Matrix2D_Float));
    if(ptr)
    {
        ptr->mat_float = malloc(m * n * sizeof(float));
        ptr->m = m;
        ptr->n = n;
        if(ptr->mat_float) {
            mat2Dfloat_fillzeros(ptr);
        }
    }
    return ptr;
}


void mat2Dfloat_fillzeros(Matrix2D_Float* ptr)
{
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


float mat2Dfloat_return(const Matrix2D_Float* ptr, size_t i, size_t j)
{
    float result = 0;
    if((i < ptr->m) && (j < ptr->n)){
        result = *((ptr->mat_float) + (ptr->n)*i + j);
    }
    return result;
}


void mat2Dfloat_insert(Matrix2D_Float* ptr, size_t i, size_t j, float val)
{
    if((i < ptr->m) && (j < ptr->n)){
        *((ptr->mat_float) + (ptr->n)*i + j) = val;
    }
}


void mat2Dfloat_arithmetic(const Matrix2D_Float* a, const Matrix2D_Float* b, Matrix2D_Float* c, MathOp mode)
{
    // mode: 0 = add, 1 = sub
    if(a && b && c) {
        if((a->m == b->m) && (a->n == b->n) && (a->m == c->m) && (a->n == c->n)) {
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
}


void mat2Dfloat_matmul(const Matrix2D_Float* a, const Matrix2D_Float* b, Matrix2D_Float* c)
{
    // carries out matrix multiplication (a@b) provided
    // both of their dimensions conform, and c is correct.
    if(a && b && c) {
        if((c->m == a->m) && (c->n == b->n) && (a->n == b->m)) {
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