#include "gf251.h"
#include <stdlib.h>
#include <string.h>

/*************************************************/
/***********     MATRIX OPERATIONS    ************/
/*************************************************/

// vz[] += vx[1][] * y[1] + ... + vx[nb][] * y[nb]
// bytes must be multiple of 16
void gf251_mat16cols_muladd(uint8_t* vz, const uint8_t* y, const uint8_t* vx, uint32_t nb, uint32_t bytes) {
    gf251_matcols_muladd(vz, y, vx, nb, bytes);
}

void gf251_mat128cols_muladd(uint8_t* vz, const uint8_t* y, const uint8_t* vx, uint32_t nb, uint32_t bytes) {
    gf251_matcols_muladd(vz, y, vx, nb, bytes);
}
