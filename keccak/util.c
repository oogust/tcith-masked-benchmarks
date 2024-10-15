#include "util.h"

/* Add two numbers in the GF(2^8) finite field */
uint8_t gadd(uint8_t a, uint8_t b) {
	return a ^ b;
}

/* Multiply two numbers in the GF(2^8) finite field defined 
 * by the modulo polynomial relation x^8 + x^4 + x^3 + x + 1 = 0
 * (the other way being to do carryless multiplication followed by a modular reduction)
 */
uint8_t gmul(uint8_t a, uint8_t b) {
	uint8_t p = 0; /* accumulator for the product of the multiplication */
	while (a != 0 && b != 0) {
        if (b & 1) /* if the polynomial for b has a constant term, add the corresponding a to p */
            p ^= a; /* addition in GF(2^m) is an XOR of the polynomial coefficients */

        if (a & 0x80) /* GF modulo: if a has a nonzero term x^7, then must be reduced when it becomes x^8 */
            a = (a << 1) ^ 0x11b; /* subtract (XOR) the primitive polynomial x^8 + x^4 + x^3 + x + 1 (0b1_0001_1011) – you can change it but it must be irreducible */
        else
            a <<= 1; /* equivalent to a*x */
        b >>= 1;
	}
	return p;
}

uint8_t mult1B_compact(uint8_t a, uint8_t b) {
    uint8_t r = 0, i = 8;
    while(i)
        r = (-(b>>--i & 1) & a) ^ (-(r>>7) & 0x1B) ^ (r+r);
    return r;
}

uint8_t mult1B_fast(uint8_t a, uint8_t b) {
    uint8_t r;
    r = (-(b>>7    ) & a);
    r = (-(b>>6 & 1) & a) ^ (-(r>>7) & 0x1B) ^ (r+r);
    r = (-(b>>5 & 1) & a) ^ (-(r>>7) & 0x1B) ^ (r+r);
    r = (-(b>>4 & 1) & a) ^ (-(r>>7) & 0x1B) ^ (r+r);
    r = (-(b>>3 & 1) & a) ^ (-(r>>7) & 0x1B) ^ (r+r);
    r = (-(b>>2 & 1) & a) ^ (-(r>>7) & 0x1B) ^ (r+r);
    r = (-(b>>1 & 1) & a) ^ (-(r>>7) & 0x1B) ^ (r+r);
 return (-(b    & 1) & a) ^ (-(r>>7) & 0x1B) ^ (r+r);
}

uint8_t mult1B_shift8(uint8_t a, uint8_t b) {
    uint16_t r,s;
        r  = (-((s = b+b)&256))>>8 & a; r += r; r ^= (-(r&256))>>8 & 0x1B;
        r ^= (-((s += s )&256))>>8 & a; r += r; r ^= (-(r&256))>>8 & 0x1B;
        r ^= (-((s += s )&256))>>8 & a; r += r; r ^= (-(r&256))>>8 & 0x1B;
        r ^= (-((s += s )&256))>>8 & a; r += r; r ^= (-(r&256))>>8 & 0x1B;
        r ^= (-((s += s )&256))>>8 & a; r += r; r ^= (-(r&256))>>8 & 0x1B;
        r ^= (-((s += s )&256))>>8 & a; r += r; r ^= (-(r&256))>>8 & 0x1B;
        r ^= (-((s += s )&256))>>8 & a; r += r; r ^= (-(r&256))>>8 & 0x1B;
 return r ^( (-((s +  s )&256))>>8 & a);
}

uint8_t add(uint8_t a, uint8_t b) {
    return gadd(a, b);
}

uint8_t mult(uint8_t a, uint8_t b) {
    return gmul(a, b);
}

uint8_t pow_mod(uint8_t x, uint8_t n) {
    uint8_t res = 1;
    while(n > 0) {
        uint8_t k = (n & 0x1);
        res = k * mult(res, x) + (1-k) * res;
        n >>= 1;
        x = mult(x, x);
    }
    return res;
}

uint8_t exp_254(uint8_t x) {
    uint8_t w, y, z;
    z = pow_mod(x, 2);
    y = mult(z, x);
    w = pow_mod(y, 4);
    y = mult(y, w);
    y = pow_mod(y, 16);
    y = mult(y, w);
    y = mult(y, z);
    return y;
}

uint8_t inv(uint8_t a) {
    return pow_mod(a, 254);
}

void mask(uint8_t* x_masked, uint8_t x, uint8_t d) {
    uint8_t sum = 0;
    for(int i = 1; i < d+1; i++) {
        x_masked[i] = rand()&0xFF; 
        sum = add(sum, x_masked[i]);
    }
    x_masked[0] = add(x, sum);
}

uint8_t unmask(uint8_t* x_masked, uint8_t d) {
    uint8_t sum = 0;
    for(int i = 0; i < d+1; i++) {
        sum = add(sum, x_masked[i]);
    }
    return sum;
}

void mask_mult(uint8_t* x_masked, uint8_t x, uint8_t d) {
    uint8_t prod = 1;
    for(int i = 1; i < d+1; i++) {
        x_masked[i] = rand()&0xFF; 
        while(x_masked[i] == 0) x_masked[i] = rand()&0xFF;
        prod = mult(prod, x_masked[i]);
    }
    x_masked[0] = mult(x, prod);
}

uint8_t unmask_mult(uint8_t* x_masked, uint8_t d) {
    uint8_t prod = x_masked[0];
    for(int i = 1; i < d+1; i++) {
        prod = mult(prod, inv(x_masked[i]));
    }
    return prod;
}

void vec_add(uint8_t* a, uint8_t* b, uint8_t* c, uint8_t d) {
    for(int i = 0; i < d+1; i++) {
        c[i] = add(a[i], b[i]);
    }
}

void vec_mult(uint8_t* a, uint8_t* b, uint8_t* c, uint8_t d) {
    for(int i = 0; i < d+1; i++) {
        c[i] = mult(a[i], b[i]);
    }
}

void sec_mult(uint8_t* a, uint8_t* b, uint8_t* c, uint8_t d) {
    uint8_t *r = (uint8_t*) malloc((d+1)*(d+1)*sizeof(uint8_t));
    for(int i = 0; i < d+1; i++) {
        for(int j = i+1; j < d+1; j++) {
            r[i*(d+1) + j] = rand()&0xFF;
            r[j*(d+1) + i] = add(add(r[i*(d+1) + j], mult(a[i], b[j])), mult(a[j], b[i]));
        }
    }
    for(int i = 0; i < d+1; i++) {
        c[i] = mult(a[i], b[i]);
        for(int j = 0; j < d+1; j++) {
            if(j != i) {
                c[i] = add(c[i], r[i*(d+1) + j]);
            }
        }
    }
    free(r);
}

void sec_AMtoMM(uint8_t* x, uint8_t* z, uint8_t d) {
    z[0] = x[0];
    for(int i = 1; i < d+1; i++) {
        z[i] = rand()&0xFF;
        while(z[i] == 0) z[i] = rand()&0xFF;

        z[0] = mult(z[0], z[i]);

        for(int j = 1; j < (d+1) - i; j++) {
            uint8_t U = rand()&0xFF;
            x[j] = mult(z[i], x[j]);
            x[j] = add(x[j], U);
            z[0] = add(z[0], x[j]);
            x[j] = U;
        }

        x[d-i+1] = mult(z[i], x[d-i+1]);
        z[0] = add(z[0], x[d-i+1]);
    }
}

void sec_MMtoAM(uint8_t* z, uint8_t* x, uint8_t d) {
    x[0] = z[0];
    for(int i = 1; i < (d+1); i++) {
        x[i] = rand()&0xFF;
        x[0] = add(x[0], x[i]);
        x[0] = mult(x[0], inv(z[i]));
        for(int j = 1; j < (i+1); j++) {
            x[j] = mult(x[j], inv(z[i]));
            uint8_t U = rand()&0xFF;
            x[j] = add(x[j], U);
            x[0] = add(x[0], x[j]);
            x[j] = U;
        }
    }
}

void refresh_masks_quadratic(uint8_t* a, uint8_t d) {
    // Secure composition
    for(int i = 0; i < d; i++) {
        for(int j = i+1; j < (d+1); j++) {
            uint8_t r = rand()&0xFF; 
            a[i] = add(a[i], r);
            a[j] = add(a[j], r); 
        }
    }
}

void refresh_masks_quasi_linear(uint8_t* a, uint8_t d) {
    
}

