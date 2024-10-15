#ifndef MULT_H
#define MULT_H

#include <stdint.h>
#include "gf251.h"
#include "gf256.h"
#include "rng.h"
#include "masking.h"
#include "util.h"

void sec_mult(uint8_t c[NSHARES], uint8_t a[NSHARES], uint8_t b[NSHARES]);
void PINI_sec_mult(uint8_t c[NSHARES], uint8_t a[NSHARES], uint8_t b[NSHARES]);

#endif