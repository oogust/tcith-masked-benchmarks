#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <string.h>
#include "rng.h"
#include "masking.h"

void rand_vec(int len, uint8_t* dest);
void mask(uint8_t x_masked[NSHARES], uint8_t x);
void unmask(uint8_t x_masked[NSHARES], uint8_t *x);
void mask_vec(int len, uint8_t x_masked_vec[NSHARES * len], uint8_t x_vec[len]);
void unmask_vec(int len, uint8_t x_masked_vec[NSHARES * len], uint8_t x_vec[len]);
void print_vec(int len, uint8_t x_vec[len]);

/*
void rand_vec_64(int len, uint64_t* dest) {
    for(int i = 0; i < len; i++) {
        dest[i] = rand64();
    }
}

void mask_vec_64(int len, uint64_t x_masked_vec[NSHARES * len], uint64_t x_vec[len]) {
    for (int i = 0; i < len; i++) {
        uint8_t sum = 0;
        for(int j = 1; j < NSHARES; j++) {
            x_masked_vec[j * len + i] = rand()%0xFF;
            sum = sum ^ x_masked_vec[j * len + i];
        }
        x_masked_vec[0 * len + i] = x_vec[i] ^ sum; 
    }
}

void unmask_vec_64(int len, uint64_t x_masked_vec[NSHARES * len], uint64_t x_vec[len]) {
    for (int i = 0; i < len; i++) {
        x_vec[i] = 0;
        for(int j = 0; j < NSHARES; j++) {
            x_vec[i] = x_vec[i] ^ x_masked_vec[j * len + i];
        }
    }
}

void print_vec_64(int len, uint64_t x_vec[len]) {
    for(int i = 0; i < len; i++) {
        printf("%lu ", x_vec[i]);
    }
    printf("\n");
}
*/

// BITSLICE GADGETS
// Atomic gadgets

void copy_sharing(size_t nshares, uint32_t *out, size_t out_stride,
                  const uint32_t *in, size_t in_stride);
void masked_xor_c(size_t nshares, uint32_t *out, size_t out_stride,
                const uint32_t *ina, size_t ina_stride, const uint32_t *inb,
                size_t inb_stride);
void masked_and_c(size_t nshares, uint32_t *z, size_t z_stride, const uint32_t *a,
                size_t a_stride, const uint32_t *b, size_t b_stride);

void copy_sharing_c(size_t nshares, uint32_t *out, size_t out_stride,
                  const uint32_t *in, size_t in_stride);

#define masked_and(nshares, z, z_stride, a, a_stride, b, b_stride)             \
  masked_and_c(nshares, z, z_stride, a, a_stride, b, b_stride)
#define masked_xor(nshares, z, z_stride, a, a_stride, b, b_stride)             \
  masked_xor_c(nshares, z, z_stride, a, a_stride, b, b_stride)
#define copy_sharing(nshares, out, out_stride, in, in_stride)                   \
  copy_sharing_c(nshares,out,out_stride,in, in_stride)

// STANDARD GADGETS

//secMult from https://www.iacr.org/archive/ches2010/62250403/62250403.pdf
void secAND(uint64_t* c, uint64_t* a, uint64_t* b);
void PINIsecAND(uint64_t *c, uint64_t *a, uint64_t *b);
void quadraticRefresh(uint64_t *a);
void quasiLinearRefresh(uint32_t n, uint64_t *a);
void quasiLinearRefresh32(uint32_t n, uint32_t *a);

#endif