/*
Filename: dynamics.c
Author:   AllAcacia
*/


#include "dynamics.h"


void dynamicSS_init(SecondOrderDTS* system, const float fn, const float xi, const float dt)
{
    system->fn = fn;
    float wn = 2 * M_PI * fn;
    system->wn = wn;
    system->xi = xi;
    system->dt = dt;

    system->F = mat2Dfloat_init(2, 2);
    mat2Dfloat_insert(&system->F, 0, 0, 1.0f);
    mat2Dfloat_insert(&system->F, 0, 1, dt);
    mat2Dfloat_insert(&system->F, 1, 0, -(dt*wn*wn));
    mat2Dfloat_insert(&system->F, 1, 1, -(2*dt*wn*xi)+1.0f);

    system->G = mat2Dfloat_init(2, 1);
    mat2Dfloat_insert(&system->G, 1, 0, dt);

    system->H = mat2Dfloat_init(1, 2);
    mat2Dfloat_insert(&system->H, 0, 0, system->wn);

    system->x1 = mat2Dfloat_init(2, 1); // stores the base and first order measurements of the output. Automatically 0'd.
    system->x2 = mat2Dfloat_init(2, 1); // stores the base and first order measurements of the output. Automatically 0'd.
    system->u1 = mat2Dfloat_init(2, 1); // is the 2x1 vector, G, but linearly scaled to an input value.
}


// void dynamicSS_del(SecondOrderDTS* system)
// {
//     mat2Dfloat_del(system->F);
//     mat2Dfloat_del(system->G);
//     mat2Dfloat_del(system->H);
//     mat2Dfloat_del(system->x1);
//     mat2Dfloat_del(system->x2);
//     mat2Dfloat_del(system->u1);
//     free(system);
// }


void dynamicSS_setstate(SecondOrderDTS* system, float val1, float val2)
{
    mat2Dfloat_insert(&system->x1, 0, 0, val1);
    mat2Dfloat_insert(&system->x1, 1, 0, val2);
}


void dynamicSS_iterate(SecondOrderDTS* system, const float u)
{
    // Iterates the system.
    mat2Dfloat_scale(&system->G, &system->u1, u);
    mat2Dfloat_matmul(&system->F, &system->x1, &system->x2);
    mat2Dfloat_arithmetic(&system->x2, &system->u1, &system->x1, OP_ADD);

    system->y = mat2Dfloat_return(&system->H, 0, 0) * mat2Dfloat_return(&system->x1, 0, 0);
}