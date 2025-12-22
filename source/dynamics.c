/*
Filename: dynamics.c
Author:   AllAcacia
*/


#include "dynamics.h"


SecondOrderDTS* dynamicSS_init(const float fn, const float xi, const float dt)
{
    SecondOrderDTS* system = malloc(sizeof(SecondOrderDTS));
    system->fn = fn;
    float wn = 2 * M_PI * fn;
    system->wn = wn;
    system->xi = xi;
    system->dt = dt;

    system->F = mat2Dfloat_init(2, 2);
    mat2Dfloat_insert(system->F, 0, 0, 1.0f);
    mat2Dfloat_insert(system->F, 0, 1, dt);
    mat2Dfloat_insert(system->F, 1, 0, -(dt*wn*wn));
    mat2Dfloat_insert(system->F, 1, 1, -(2*dt*wn*xi)+1.0f);

    system->G = mat2Dfloat_init(2, 1);
    mat2Dfloat_insert(system->G, 1, 0, 1);

    system->H = mat2Dfloat_init(1, 2);
    mat2Dfloat_insert(system->H, 0, 0, system->wn);

    system->x1 = mat2Dfloat_init(2, 1); // stores the base and first order measurements of the output. Automatically 0'd.
    system->u1 = mat2Dfloat_init(1, 1); // essentially a scalar unless later we desire first order measurement. Automatically 0'd.

    return system;
}