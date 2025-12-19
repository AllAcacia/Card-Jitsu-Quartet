/*
Filename: dynamics.h
Author:   AllAcacia
*/


#include "3ds.h" // for u64
#include "matrix.h"


typedef struct {
    float v;  // state-space element 1
    float dv; // state-space element 2
    
    float a0; // "A" matrix 0th derivative coefficient
    float a1; // "A" matrix 1st derivative coefficient
    
    float kp; // proportional gain
    float kd; // derivative gain
    float ki; // integral gain

    u64   dt; // frequency of iteration updates
} SecondOrderDTS;