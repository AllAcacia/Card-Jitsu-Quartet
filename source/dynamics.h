/*
Filename: dynamics.h
Author:   AllAcacia
*/


#include "3ds.h" // for u64
#include "mat2d.h"


typedef struct {
    float wn; // natural frequency
    float xi; // damping coefficient
    float dt; // frequency of iteration updates

    Matrix2D_Float* A; // main EoM in CCF
    Matrix2D_Float* B; // input with optional CL feedback
    Matrix2D_Float* x1; // 2x1 vector of displacement and velocity (k)
    Matrix2D_Float* x2; // 2x1 vector of displacement and velocity (k+1), intended to be swapped pointrs with x1
} SecondOrderDTS;