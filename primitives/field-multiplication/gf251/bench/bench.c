#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "timing.h"

void p251_vec_mat128cols_muladd_avx2_ct(void *vz, uint8_t const *vx, void const *my, uint64_t m, uint64_t scaling);

#define NB_TESTS_1 100000
#define NB_TESTS_2 100
#define LENGTH 93

void randombytes(unsigned char* x, unsigned long long xlen) {
    for(unsigned long long j=0; j<xlen; j++)
        x[j] = rand();
}

int main() {
    srand((unsigned int) time(NULL));

    uint8_t output[128];
    uint8_t vx[LENGTH];
    uint8_t my[LENGTH][128];
    

    btimer_t timer;
    for(unsigned int i=0; i<NB_TESTS_1; i++) {
        randombytes(vx, LENGTH);
        randombytes((uint8_t*) my, LENGTH*128);

        btimer_init(&timer);
        btimer_start(&timer);
        for(unsigned int j=0; j<NB_TESTS_2; j++)
            p251_vec_mat128cols_muladd_avx2_ct(output, vx, my, LENGTH, 1);
        btimer_end(&timer);
        for(unsigned int j=0; j<NB_TESTS_2; j++)
            btimer_count(&timer);
    }
    double timing = btimer_get(&timer);

    printf("Timing: %f ms\n", timing);

    return 0;
}