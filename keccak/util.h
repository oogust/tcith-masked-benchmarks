#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

uint8_t add(uint8_t a, uint8_t b);
uint8_t mult(uint8_t a, uint8_t b);
uint8_t inv(uint8_t a);
void mask(uint8_t* x_masked, uint8_t x, uint8_t d);
uint8_t unmask(uint8_t* x_masked, uint8_t d); 
uint8_t unmask_mult(uint8_t* x_masked, uint8_t d);

void vec_add(uint8_t* a, uint8_t* b, uint8_t* c, uint8_t d);
void vec_mult(uint8_t* a, uint8_t* b, uint8_t* c, uint8_t d);
void sec_mult(uint8_t* a, uint8_t* b, uint8_t* c, uint8_t d);

void sec_AMtoMM(uint8_t* x, uint8_t* z, uint8_t d);
void sec_MMtoAM(uint8_t* z, uint8_t* x, uint8_t d);

void refresh_masks_quadratic(uint8_t* a, uint8_t d);


#endif