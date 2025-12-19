/*
Filename: matrix.h
Author:   AllAcacia
*/


#include <stdint.h>
#include <stdlib.h>


typedef struct {
    uint8_t m; // rows
    uint8_t n; // cols
    float* mat_float;
} Matrix2D_Float;


Matrix2D_Float* mat2Dfloat_init(uint8_t m, uint8_t n);