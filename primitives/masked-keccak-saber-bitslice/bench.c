#include "masked_fips202.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h> 
#include <string.h>
#include "util.h"
#include "rng.h"
#include "masking.h"
//#include "masked_representations.h"

#define IN_LEN 32
#define OUT_LEN 32

#define NSAMPLES 1

int main() {
    uint8_t input[IN_LEN];
    uint8_t output[OUT_LEN];

    uint8_t input_masked[NSHARES * IN_LEN];
    uint8_t output_masked[NSHARES * OUT_LEN];

    uint8_t input_masked_bs[NSHARES * IN_LEN];
    uint8_t output_masked_bs[NSHARES * OUT_LEN];

    rand_vec(IN_LEN, input);
    mask_vec(IN_LEN, input_masked, input);

    for(int i = 0; i < NSAMPLES; i++) {
        size_t outlen = (size_t) OUT_LEN;
        size_t out_msk_stride = (size_t) OUT_LEN;
        size_t out_data_stride = (size_t) 1;

        size_t inlen = (size_t) IN_LEN;
        size_t in_msk_stride = (size_t) OUT_LEN;
        size_t in_data_stride = (size_t) 1;

        masked_shake128(output_masked, outlen, out_msk_stride,
            out_data_stride, input, inlen,
            in_msk_stride, in_data_stride);
    }
    unmask_vec(OUT_LEN, output_masked, output);

    print_vec(IN_LEN, input);
    unmask_vec(IN_LEN, input_masked, input);
    print_vec(IN_LEN, input);

    print_vec(OUT_LEN, output);

    return 0;
}