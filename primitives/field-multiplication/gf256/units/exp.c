#include "gf256-constant.h"

// This is a test-only function (not constant time)
const int overflow = 0x100, modulus = 0x11B;  // this is the standard AES GF(2^8) representation

int gf2n_multiply(int a, int b) {
    int sum = 0;
    while (b > 0) {
        if (b & 1) sum = sum ^ a;             // if last bit of b is 1, add a to the sum
        b = b >> 1;                           // divide b by 2, discarding the last bit
        a = a << 1;                           // multiply a by 2
        if (a & overflow) a = a ^ modulus;    // reduce a modulo the AES polynomial
    }
    return sum;
}


uint8_t mult1B_compact(uint8_t a, uint8_t b) {
    uint8_t r = 0, i = 8;
    while(i)
        r = (-(b>>--i & 1) & a) ^ (-(r>>7) & 0x1B) ^ (r+r);
    return r;
}

// Constant time C portable GF256 multiplication
// We use 0x14D for the modulus instead of 0x11B. [TO DO: to change]
#define MODULUS 0x4D
#define MASK_LSB_PER_BIT ((uint64_t)0x0101010101010101)
#define MASK_MSB_PER_BIT (MASK_LSB_PER_BIT*0x80)
#define MASK_XLSB_PER_BIT (MASK_LSB_PER_BIT*0xFE)

uint8_t mult1B_fast(uint8_t a, uint8_t b) {
    uint8_t r;
    r = (-(b>>7    ) & a);
    r = (-(b>>6 & 1) & a) ^ (-(r>>7) & MODULUS) ^ (r+r);
    r = (-(b>>5 & 1) & a) ^ (-(r>>7) & MODULUS) ^ (r+r);
    r = (-(b>>4 & 1) & a) ^ (-(r>>7) & MODULUS) ^ (r+r);
    r = (-(b>>3 & 1) & a) ^ (-(r>>7) & MODULUS) ^ (r+r);
    r = (-(b>>2 & 1) & a) ^ (-(r>>7) & MODULUS) ^ (r+r);
    r = (-(b>>1 & 1) & a) ^ (-(r>>7) & MODULUS) ^ (r+r);
 return (-(b    & 1) & a) ^ (-(r>>7) & MODULUS) ^ (r+r);
}

uint64_t mult8B_fast(uint64_t a, uint8_t b) {
    uint64_t r;
    r = ((-(uint64_t)(b>>7    )) & a);
    r = ((-(uint64_t)(b>>6 & 1)) & a) ^ (((r>>7) & MASK_LSB_PER_BIT) * MODULUS) ^ ((r+r) & MASK_XLSB_PER_BIT);
    r = ((-(uint64_t)(b>>5 & 1)) & a) ^ (((r>>7) & MASK_LSB_PER_BIT) * MODULUS) ^ ((r+r) & MASK_XLSB_PER_BIT);
    r = ((-(uint64_t)(b>>4 & 1)) & a) ^ (((r>>7) & MASK_LSB_PER_BIT) * MODULUS) ^ ((r+r) & MASK_XLSB_PER_BIT);
    r = ((-(uint64_t)(b>>3 & 1)) & a) ^ (((r>>7) & MASK_LSB_PER_BIT) * MODULUS) ^ ((r+r) & MASK_XLSB_PER_BIT);
    r = ((-(uint64_t)(b>>2 & 1)) & a) ^ (((r>>7) & MASK_LSB_PER_BIT) * MODULUS) ^ ((r+r) & MASK_XLSB_PER_BIT);
    r = ((-(uint64_t)(b>>1 & 1)) & a) ^ (((r>>7) & MASK_LSB_PER_BIT) * MODULUS) ^ ((r+r) & MASK_XLSB_PER_BIT);
 return ((-(uint64_t)(b    & 1)) & a) ^ (((r>>7) & MASK_LSB_PER_BIT) * MODULUS) ^ ((r+r) & MASK_XLSB_PER_BIT);
}

uint8_t mult1B_with_public(uint8_t a, uint8_t b_public) {
    // Experimental: it does not seem to be faster than "mult1B_fast"
    uint8_t acc = 0, carry;
    while (b_public > 0) {
        if (b_public & 1) acc = acc ^ a;      // if last bit of b is 1, add a to the sum
        b_public = b_public >> 1;             // divide b by 2, discarding the last bit
        carry = a & 0x80;
        a = a << 1;                           // multiply a by 2
        if(carry) a = a ^ MODULUS;    // reduce a modulo the AES polynomial
    }
    return acc;
}

uint64_t mult8B_with_public(uint8_t a_public, uint64_t b) {
    // Experimental: it does not seem to be faster than "mult8B_fast"
    uint64_t acc = 0, carry;
    while (a_public > 0) {
        if (a_public & 1) acc = acc ^ b;
        a_public = a_public >> 1;
        carry = ((b>>7) & MASK_LSB_PER_BIT) * MODULUS;
        b = (b & MASK_MSB_PER_BIT) << 1;
        b = (b << 1) ^ carry;
    }
    return acc;
}

uint8_t mult1B_shift8(uint8_t a, uint8_t b) {
    uint16_t r,s;
        r  = (-((s = b+b)&256))>>8 & a; r += r; r ^= (-(r&256))>>8 & MODULUS;
        r ^= (-((s += s )&256))>>8 & a; r += r; r ^= (-(r&256))>>8 & MODULUS;
        r ^= (-((s += s )&256))>>8 & a; r += r; r ^= (-(r&256))>>8 & MODULUS;
        r ^= (-((s += s )&256))>>8 & a; r += r; r ^= (-(r&256))>>8 & MODULUS;
        r ^= (-((s += s )&256))>>8 & a; r += r; r ^= (-(r&256))>>8 & MODULUS;
        r ^= (-((s += s )&256))>>8 & a; r += r; r ^= (-(r&256))>>8 & MODULUS;
        r ^= (-((s += s )&256))>>8 & a; r += r; r ^= (-(r&256))>>8 & MODULUS;
 return r ^( (-((s +  s )&256))>>8 & a);
}

void mult1B_fast_tab(uint8_t* vz, const uint8_t* vx, uint8_t y, int bytes) {
    for(int i=0; i<bytes; i++)
        vz[i] = mult1B_fast(vx[i], y);
}

void mult1B_fast_add_tab(uint8_t* vz, uint8_t y, const uint8_t* vx, int bytes) {
    for(int i=0; i<bytes; i++)
        vz[i] = vz[i] ^ mult1B_fast(vx[i], y);
}



#if 0
// WORK IN PROGRESS

#define GF256_BATCH_MULT(type,parallel,dbl) {        \
    type* _loc_cur_vz = (type*)cur_vz;                \
    type* _loc_cur_vx = (type*)cur_vx;                \
    type r;                                           \
    while (i >= parallel) {                           \
      r = ((-(uint64_t)(b>>7    )) & a);              \

    uint64_t r;
    r = ((_b[0]) & a);
    r = ((_b[1]) & a) ^ (((r>>7) & MASK_LSB_PER_BIT) * MODULUS) ^ (dbl(r,r));
    r = ((_b[2]) & a) ^ (((r>>7) & MASK_LSB_PER_BIT) * MODULUS) ^ (dbl(r,r));
    r = ((_b[3]) & a) ^ (((r>>7) & MASK_LSB_PER_BIT) * MODULUS) ^ (dbl(r,r));
    r = ((_b[4]) & a) ^ (((r>>7) & MASK_LSB_PER_BIT) * MODULUS) ^ (dbl(r,r));
    r = ((_b[5]) & a) ^ (((r>>7) & MASK_LSB_PER_BIT) * MODULUS) ^ (dbl(r,r));
    r = ((_b[6]) & a) ^ (((r>>7) & MASK_LSB_PER_BIT) * MODULUS) ^ (dbl(r,r));
 return ((_b[7]) & a) ^ (((r>>7) & MASK_LSB_PER_BIT) * MODULUS) ^ (dbl(r,r));

      _vz++; _vx++;                                   \
      i -= parallel;                                  \
    }                                                 \
    cur_v = _loc_cur_v;                               \
    }

#define _mm1_add(a,b) (a+b)

void mult1B_fast_tab(uint8_t* vz, const uint8_t* vx, uint8_t y, int bytes) {
    void* cur_vz  = (void*) vz;
    void* cur_vx  = (void*) vx;
    int i = bytes;

  #ifdef __AVX512__
    GF256_BATCH_MULT(__m512i, 64, _mm512_setzero_si512(), _mm512_add_epi8);
  #endif
  #ifdef __AVX__
    GF256_BATCH_MULT(__m256i, 32, _mm256_setzero_si256(), _mm256_add_epi8);
  #endif
  #ifdef __SSE__
    GF256_BATCH_MULT(__m128i, 16, _mm_setzero_si128(), _mm_add_epi8);
  #endif
    GF256_BATCH_MULT(uint64_t, 8, 0, _mm_add_pi8);
    GF256_BATCH_MULT(uint8_t, 1, 0, _mm1_add);
}
#endif

