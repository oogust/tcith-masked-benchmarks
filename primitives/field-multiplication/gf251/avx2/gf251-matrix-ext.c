#include "gf251.h"
#include <stdlib.h>
#include <string.h>

#include "gf251-internal.h"

/*************************************************/
/***********     MATRIX OPERATIONS    ************/
/*************************************************/

void p251_vec_mat16cols_muladd_b16_avx2_ct(void *vz, uint8_t const *vx,
                                                  void const *my, uint64_t m, uint64_t scaling);
// vz[] += vx[1][] * y[1] + ... + vx[nb][] * y[nb]
// bytes must be multiple of 16
void gf251_mat16cols_muladd(uint8_t* vz, const uint8_t* y, const uint8_t* vx, uint32_t nb, uint32_t bytes) {
    gf251_matcols_muladd(vz, y, vx, nb, bytes);
}

void p251_vec_mat128cols_muladd_avx2_ct(void *vz, uint8_t const *vx,
                                               void const *my, uint64_t m, uint64_t scaling);

void gf251_mat128cols_muladd(uint8_t* vz, const uint8_t* y, const uint8_t* vx, uint32_t nb, uint32_t bytes) {
    uint32_t nb_packs = bytes>>7;
    for(uint32_t i=0; i<nb_packs; i++)
        p251_vec_mat128cols_muladd_avx2_ct(vz+128*i, y, vx+128*i, nb, nb_packs);
}
