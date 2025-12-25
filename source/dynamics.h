/*
Filename: dynamics.h
Author:   AllAcacia
*/


#include "3ds.h" // for u64
#include "mat2d.h"
#include <math.h>


typedef struct {
    float y;  // measured output

    float fn; // natural frequency
    float wn; // natural (angular) frequency
    float xi; // damping coefficient
    float dt; // interval of iteration updates

    Matrix2D_Float F; // main EoM in CCF
    Matrix2D_Float G; // input with optional CL feedback
    Matrix2D_Float H; // output matrix [wn**2, 0]

    Matrix2D_Float x1; // 2x1 vector of displacement and velocity (k)
    Matrix2D_Float x2; // 2x1 vector of displacement and velocity (k+1), intended to be swapped pointers with x1
    Matrix2D_Float u1; // 2x1 vector that holds input information. Scaled version of G.
} SecondOrderDTS;


void dynamicSS_init(SecondOrderDTS* system, const float fn, const float xi, const float dt);

// void dynamicSS_del(SecondOrderDTS* system);

void dynamicSS_setstate(SecondOrderDTS* system, float val1, float val2);

void dynamicSS_iterate(SecondOrderDTS* system, const float u);