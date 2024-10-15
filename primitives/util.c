#include "util.h"

void rand_vec(int len, uint8_t* dest) {
    for(int i = 0; i < len; i++) {
        dest[i] = rand()%0xFF;
    }
}

void mask(uint8_t x_masked[NSHARES], uint8_t x) {
    unsigned int i;
    uint8_t sum = 0;
    for(i = 1; i < NSHARES; i++) {
        x_masked[i] = rand()&0XFF;
        sum ^= x_masked[i];
    }
    x_masked[0] = x ^ sum;
}

void unmask(uint8_t x_masked[NSHARES], uint8_t *x) {
    unsigned int i;
    *x = 0;
    for(i = 0; i < NSHARES; i++) {
        *x ^= x_masked[i];
    }
}

void mask_vec(int len, uint8_t x_masked_vec[NSHARES * len], uint8_t x_vec[len]) {
    for (int i = 0; i < len; i++) {
        uint8_t sum = 0;
        for(int j = 1; j < NSHARES; j++) {
            x_masked_vec[j * len + i] = rand()&0XFF;
            sum = sum ^ x_masked_vec[j * len + i];
        }
        x_masked_vec[0 * len + i] = x_vec[i] ^ sum; 
    }
}

void unmask_vec(int len, uint8_t x_masked_vec[NSHARES * len], uint8_t x_vec[len]) {
    for (int i = 0; i < len; i++) {
        x_vec[i] = 0;
        for(int j = 0; j < NSHARES; j++) {
            x_vec[i] = x_vec[i] ^ x_masked_vec[j * len + i];
        }
    }
}

void print_vec(int len, uint8_t x_vec[len]) {
    for(int i = 0; i < len; i++) {
        printf("%2x ", x_vec[i]);
    }
    printf("\n");
}


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

static inline uint32_t pini_and_core(uint32_t a, uint32_t b, uint32_t r) {
  uint32_t temp;
  uint32_t s;
  //__asm__("eor %[temp], %[b], %[r]\n\t"
  //    "and %[temp], %[a], %[temp]\n\t"
  //    "bic %[s], %[r], %[a]\n\t"
  //    "eor %[s], %[s], %[temp]"
  //    : [ s ] "=r"(s),
  //      [ temp ] "=&r"(
  //          temp) /* outputs, use temp as an arbitrary-location clobber */
  //    : [ a ] "r"(a), [ b ] "r"(b), [ r ] "r"(r) /* inputs */
  //);
  temp = b ^ r;
  temp = a & temp;
  s = r & ~a;
  s = s ^ temp;
  //s = a & b;
  //printf("%d\n", s);
  //printf("%d\n", a & b);
  return s;
}


// BITSLICE GADGETS

/*************************************************
 * Name:        masked_and
 *
 * Description: Performs masked AND (z = a & b ) gate with nshares.
 *
 * Arguments:   - size_t nshares: number of shares
 *            - uint32_t *z: output buffer
 *            - size_t z_stride: output buffer stride
 *            - uint32_t *a: first input buffer
 *            - size_t a_stride: a buffer stride
 *            - uint32_t *b: second input buffer
 *            - size_t b_stride: b buffer stride
 **************************************************/
void masked_and_c(size_t nshares, uint32_t *z, size_t z_stride, const uint32_t *a,
                size_t a_stride, const uint32_t *b, size_t b_stride) {
  uint32_t ztmp[nshares];
  uint32_t r;
  uint32_t i, j;

  for (i = 0; i < nshares; i++) {
    ztmp[i] = a[i * a_stride] & b[i * b_stride];
  }

  for (i = 0; i < (nshares - 1); i++) {
    for (j = i + 1; j < nshares; j++) {
      r = rand32();
      // PINI
      ztmp[i] ^= pini_and_core(a[i * a_stride], b[j * b_stride], r);
      ztmp[j] ^= pini_and_core(a[j * a_stride], b[i * b_stride], r);
    }
  }
  for (i = 0; i < nshares; i++) {
    z[i * z_stride] = ztmp[i];
  }
}

/*************************************************
 * Name:        masked_xor
 *
 * Description: Performs masked XOR (z = a ^ b ) gate with nshares.
 *
 * Arguments:   - size_t nshares: number of shares
 *            - uint32_t *z: output buffer
 *            - size_t z_stride: output buffer stride
 *            - uint32_t *a: first input buffer
 *            - size_t a_stride: a buffer stride
 *            - uint32_t *b: second input buffer
 *            - size_t b_stride: b buffer stride
 **************************************************/
void masked_xor_c(size_t nshares, uint32_t *out, size_t out_stride,
                const uint32_t *ina, size_t ina_stride, const uint32_t *inb,
                size_t inb_stride) {
  for (size_t i = 0; i < nshares; i++) {
    out[i * out_stride] = ina[i * ina_stride] ^ inb[i * inb_stride];
  }
}

/*************************************************
 * Name:        copy_sharing
 *
 * Description: Copy input sharing to output sharing 
 *
 * Arguments: - size_t nshares: number of shares
 *            - uint32_t *out: output buffer
 *            - size_t out_stride: out buffer stride
 *            - uint32_t *in: input buffer
 *            - size_t in_stride: in buffer stride
 **************************************************/
void copy_sharing_c(size_t nshares, uint32_t *out, size_t out_stride,
                  const uint32_t *in, size_t in_stride) {
  for (size_t i = 0; i < nshares; i++) {
    out[i * out_stride] = in[i * in_stride];
  }
}

// STANDARD GADGETS

//secMult from https://www.iacr.org/archive/ches2010/62250403/62250403.pdf
void secAND(uint64_t* c, uint64_t* a, uint64_t* b)
{
    unsigned int i, j, offset_i, offset_j;
    uint64_t r_ij[NSHARES * NSHARES];
    memset(r_ij, 0, NSHARES * NSHARES * 8);
    for (i = 0; i < NSHARES; i++)
    {
        offset_i = i * NSHARES;
        for (j = i + 1; j < NSHARES; j++)
        {
            offset_j = j * NSHARES;
            r_ij[j + offset_i] = rand64();
            r_ij[i + offset_j] = (a[i] & b[j]);
            r_ij[i + offset_j] = r_ij[i + offset_j] ^ r_ij[j + offset_i];
            r_ij[i + offset_j] = r_ij[i + offset_j] ^ (a[j] & b[i]);
        }
    }
    for (i = 0; i < NSHARES; i++)
    {
        c[i] = a[i] & b[i];
        offset_i = i * NSHARES;
        for (j = 0; j < NSHARES; j++)
        {
            if (i != j)
                c[i] = c[i] ^ r_ij[j + offset_i];
        }
    }
    return;
}

void PINIsecAND(uint64_t *c, uint64_t *a, uint64_t *b) {
    unsigned int i, j, offset_i, offset_j;
    uint64_t r_ij[NSHARES * NSHARES];
    uint64_t z_ij[NSHARES * NSHARES];
    uint64_t s, p0, p1;
    memset(r_ij, 0, NSHARES * NSHARES * 8);
    memset(z_ij, 0, NSHARES * NSHARES * 8);
    for(i = 0; i < NSHARES; i++) {
        offset_i = i * NSHARES;
        for(j = i+1; j < NSHARES; j++) {
            offset_j = j * NSHARES;
            r_ij[offset_i + j] = rand64();
            r_ij[offset_j + i] = r_ij[offset_i + j];
        }
    }
    for(i = 0; i < NSHARES; i++) {
        offset_i = i * NSHARES;
        for(j = 0; j < NSHARES; j++) {
            offset_j = j * NSHARES;
            if(j != i) {
                s = b[j] ^ r_ij[offset_i + j];
                p0 = (a[i] ^ 1) & r_ij[offset_i + j];
                p1 = a[i] & s;
                z_ij[offset_i + j] = p0 ^ p1;
            }
        }
    }
    for(i = 0; i < NSHARES; i++) {
        offset_i = i * NSHARES;
        c[i] = a[i] & b[i];
        for(j = 0; j < NSHARES; j++) {
            offset_j = j * NSHARES;
            if(j != i) {
                c[i] ^= z_ij[offset_i + j];
            }
        }
    }
    return;
}

void quadraticRefresh(uint64_t *a) {
    unsigned int i, j;
    uint64_t r;
    //for(i = 0; i < NSHARES; i++) {
    //    c[i] = a[i];
    //}
    for(i = 0; i < NSHARES; i++) {
        for(j = i+1; j < NSHARES; j++) {
            r = rand64();
            a[i] = a[i] ^ r;
            a[j] = a[j] ^ r;
        }
    }
    return;
}

void quasiLinearRefresh(uint32_t n, uint64_t *a) {
    unsigned int i;
    uint64_t r;
    if(n == 1) return;
    if(n == 2) {
        r = rand64();
        a[0] = a[0] ^ r;
        a[1] = a[1] ^ r;
        return;
    }
    for(i = 0; i < n/2; i++) {
        r = rand64();
        a[i] = a[i] ^ r;
        a[n/2 + i] = a[n/2 + i] ^ r;
    }
    quasiLinearRefresh(n/2, &a[0]);
    quasiLinearRefresh(n/2, &a[n/2]);
    for(i = 0; i < n/2; i++) {
        r = rand64();
        a[i] = a[i] ^ r;
        a[n/2 + i] = a[n/2 + i] ^ r;
    }
}


void quasiLinearRefresh32(uint32_t n, uint32_t *a) {
    unsigned int i;
    uint32_t r;
    if(n == 1) return;
    if(n == 2) {
        r = rand32();
        a[0] = a[0] ^ r;
        a[1] = a[1] ^ r;
        return;
    }
    for(i = 0; i < n/2; i++) {
        r = rand32();
        a[i] = a[i] ^ r;
        a[n/2 + i] = a[n/2 + i] ^ r;
    }
    quasiLinearRefresh32(n/2, &a[0]);
    quasiLinearRefresh32(n/2, &a[n/2]);
    for(i = 0; i < n/2; i++) {
        r = rand32();
        a[i] = a[i] ^ r;
        a[n/2 + i] = a[n/2 + i] ^ r;
    }
}
