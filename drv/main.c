//  main.c
//  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

//  === testing main()

#include <string.h>
#include "sloth_hal.h"
#include "api.h"

#define IN_LEN 32
#define OUT_LEN 32

#define NSAMPLES 1

#ifndef hexdump
#define hexdump(v, len, prefix) do {\
    unsigned int _i_;\
    if(prefix != NULL){\
        printf("%s: ", prefix);\
    }\
    for(_i_ = 0; _i_ < len; _i_++){\
        printf("%02x", (unsigned char)((v)[_i_]));\
    }\
    printf("\n");\
} while(0);
#endif


const char main_hello[] =
"\n[RESET]"
"\t   ______        __  __ __\n"
"\t  / __/ /  ___  / /_/ // /  SLotH Accelerator Test 2024/05\n"
"\t _\\ \\/ /__/ _ \\/ __/ _  /   SLH-DSA / FIPS 205 ipd\n"
"\t/___/____/\\___/\\__/_//_/    markku-juhani.saarinen@tuni.fi\n\n";

//  unit tests
int test_sloth();       //  test_sloth.c
int test_bench();       //  test_bench.c
int test_leak();        //  test_leak.c

#ifdef _PICOLIBC__
// XXX In case of Picolibc, redirect stdio related stuff to uart
// (see https://github.com/picolibc/picolibc/blob/main/doc/os.md)
// This allows to use printf family of functions
#include <stdio.h>
#include <stdlib.h>
static int sample_putc(char c, FILE *file)
{
        (void) file;            /* Not used in this function */
        sio_putc(c);            /* Defined by underlying system */
        return c;
}

static int sample_getc(FILE *file)
{
        unsigned char c;
        (void) file;            /* Not used in this function */
        c = sio_getc();         /* Defined by underlying system */
        return c;
}

FILE __stdio = FDEV_SETUP_STREAM(sample_putc,
                                        sample_getc,
                                        NULL,
                                        _FDEV_SETUP_RW);

FILE *const stdin = &__stdio; __strong_reference(stdin, stdout); __strong_reference(stdin, stderr);
#endif

unsigned char rr[] = {
0x67, 0xc6, 0x69, 0x73, 0x51, 0xff, 0x4a, 0xec, 0x29, 0xcd, 0xba, 0xab, 0xf2, 0xfb, 0xe3, 0x46
};
int randombytes(unsigned char* x, unsigned long long xlen) {
printf("===> Here %lld\n", xlen);
memcpy(x, rr, 16);
/*
    for(unsigned long long j=0; j<xlen; j++){
        x[j] = (uint8_t) rand();
    }
*/
    return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "util.h"
#include "rng.h"
#include "masking.h"

#include "../primitives/field-multiplication/mult.h"
#include "../primitives/masked-keccak-saber-plain/fips202.h"


int main()
{
    uint32_t start, stop;
    printf("[+] CRX tests\n");
    printf("Masking shares = %d\n", NSHARES);

    printf("\nMultiplication bench (GF256)\n");

    uint8_t a, b, c;
    uint8_t a_masked[NSHARES];
    uint8_t b_masked[NSHARES];
    uint8_t c_masked[NSHARES];

    a = 0x3;
    b = 0xb;

    mask(a_masked, a);
    mask(b_masked, b); 

    start = get_clk_ticks();
    sec_mult(c_masked, a_masked, b_masked);
    stop = get_clk_ticks(); 
    printf("ISW mult: %ld cycles\n", stop-start);

    start = get_clk_ticks();
    PINI_sec_mult(c_masked, a_masked, b_masked);
    stop = get_clk_ticks();
    printf("PINI mult: %ld cycles\n", stop-start);

    printf("\nRefresh bench \n");

    uint32_t r32[NSHARES];

    start = get_clk_ticks();
    quasiLinearRefresh32(NSHARES, r32);
    stop = get_clk_ticks();
    printf("32 bits : %ld cycles\n", stop-start);

    uint64_t r64[NSHARES];

    start = get_clk_ticks();
    quasiLinearRefresh(NSHARES, r64);
    stop = get_clk_ticks();
    printf("64 bits : %ld cycles\n", stop-start);

    printf("\nSoftware (masked) SHA3 bench\n"); 

    uint8_t input[IN_LEN];
    uint8_t output[OUT_LEN];

    uint8_t input_masked[NSHARES * IN_LEN];
    uint8_t output_masked[NSHARES * OUT_LEN];

    rand_vec(IN_LEN, input);
    mask_vec(IN_LEN, input_masked, input);

    size_t outlen = (size_t) OUT_LEN;
    size_t inlen = (size_t) IN_LEN;
    start = get_clk_ticks();
    shake128_masked(output_masked, outlen, input_masked, inlen);
    stop = get_clk_ticks();
    printf("shake128 : %ld cycles\n", stop-start);

    start = get_clk_ticks();
    sha3_256_masked(output_masked, input_masked, inlen);    
    stop = get_clk_ticks();
    printf("sha3-256 : %ld cycles\n", stop-start);

    printf("\nPlatform (unmasked) SHA3\n");

    //uint32_t res_xof[1024] = { 0 };
    //uint32_t res_hash[1024] = { 0 };
    uint8_t digest1[1024] = { 0 };
    unsigned char seed[1024] = { "a" };

    uint32_t sum = 0;
    //uint8_t digest1[32] = { 0 };
    hash_context hash_ctx1;

    start = get_clk_ticks();
    hash_init(&hash_ctx1);
    stop = get_clk_ticks();
    //printf(" [1] ==> hash_init_SHA3_256 %ld:\n", stop-start);
    sum += stop - start;

    start = get_clk_ticks();
    hash_update(&hash_ctx1, seed, inlen);
    stop = get_clk_ticks();
    //printf(" [1] ==> hash_update: %ld\n", stop-start);
    sum += stop - start;

    start = get_clk_ticks();
    hash_final(&hash_ctx1, &digest1[0]);
    stop = get_clk_ticks();
    //printf(" [1] ==> hash_final: %ld\n", stop-start);
    sum += stop - start;

    printf("\nSHA3-256: %ld cycles\n", sum);
    printf("\tinput: %d bytes\n", inlen);

    sum = 0;
    xof_context xof_ctx1;// = ctx[outlen];
        
    start = get_clk_ticks();
    xof_init(&xof_ctx1);
    stop = get_clk_ticks();
    //printf(" [1] ==> xof_init_SHAKE128 %ld:\n", stop-start);
    sum += stop - start;

    start = get_clk_ticks();
    xof_update(&xof_ctx1, seed, inlen);
    stop = get_clk_ticks();
    //printf(" [1] ==> xof_update: %ld\n", stop-start);
    sum += stop - start;

    start = get_clk_ticks();
    xof_final(&xof_ctx1);
    stop = get_clk_ticks();
    //printf(" [1] ==> xof_final: %ld\n", stop-start);
    sum += stop - start;
        
    start = get_clk_ticks();
    xof_squeeze(&xof_ctx1, &digest1[0], outlen);
    stop = get_clk_ticks();
    //printf(" [1] ==> xof_squeeze: %ld\n", stop-start);
    sum += stop - start;    

    printf("\nSHAKE-128: %ld cycles\n", sum);
    printf("\tinput: %d bytes\n", inlen);  
    printf("\tutput: %d bytes\n", outlen);  

    exit(0);
}
