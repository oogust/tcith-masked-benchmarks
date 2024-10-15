#include "fips202-masked.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h> 
#include <x86intrin.h>
#include "timing.h"

#define IN_LEN 32
#define OUT_LEN 32

#define NSAMPLES 100000


void rand_vec(int len, uint8_t* dest) {
    for(int i = 0; i < len; i++) {
        dest[i] = rand()%0xFF;
    }
}

void mask_vec(int len, uint8_t x_masked_vec[MASKING_SHARES][len], uint8_t x_vec[len]) {
    for (int i = 0; i < len; i++) {
        uint8_t sum = 0;
        for(int j = 1; j < MASKING_SHARES; j++) {
            x_masked_vec[j][i] = rand()%0xFF;
            sum = sum ^ x_masked_vec[j][i];
        }
        x_masked_vec[0][i] = x_vec[i] ^ sum; 
    }
}

void unmask_vec(int len, uint8_t x_masked_vec[MASKING_SHARES][len], uint8_t x_vec[len]) {
    for (int i = 0; i < len; i++) {
        x_vec[i] = 0;
        for(int j = 0; j < MASKING_SHARES; j++) {
            x_vec[i] = x_vec[i] ^ x_masked_vec[j][i];
        }
    }
}

void print_vec(int len, uint8_t x_vec[len]) {
    for(int i = 0; i < len; i++) {
        printf("%2x ", x_vec[i]);
    }
    printf("\n");
}

int main() {
    //uint64_t start, end;
    //unsigned int garbage;
    uint8_t input[IN_LEN];
    uint8_t output[OUT_LEN];

    uint8_t input_masked[MASKING_SHARES][IN_LEN];
    uint8_t output_masked[MASKING_SHARES][OUT_LEN];

    rand_vec(IN_LEN, input);
    mask_vec(IN_LEN, input_masked, input);

    //long long sum = 0;

    for(int i = 0; i < NSAMPLES; i++) {
        //start = __rdtscp(&garbage);
        shake128_masked_HO(OUT_LEN, output_masked, IN_LEN, input_masked);
        //end = __rdtscp(&garbage);
        //sum += end - start;
    }

    printf("%lf\n", btimer_get_cycles(&timer));
    

    unmask_vec(OUT_LEN, output_masked, output);
    print_vec(IN_LEN, input);
    print_vec(OUT_LEN, output);

    return 0;
}