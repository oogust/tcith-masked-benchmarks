#include <stdlib.h>
#include <stdio.h>
#include <stdint.h> 
#include <string.h>
#include "gf256.h"
#include "gf251.h"
#include "util.h"
#include "rng.h"
#include "masking.h"

#define NSAMPLES 1

int main() {
    uint8_t a[NSHARES];
    uint8_t b[NSHARES];
    uint8_t c[NSHARES];
    uint8_t d = NSHARES-1;

    for(int i = 0; i < NSAMPLES; i++) {
        rand_vec(NSHARES, a);
        rand_vec(NSHARES, b);
        sec_mult(c, a, b);
        printf("%x\n", c[0]);
    }
    return 0;
}
