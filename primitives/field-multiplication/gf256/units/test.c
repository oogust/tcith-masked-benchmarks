#include "gf256.h"
#include "test-include.h"
#include "debug.h"
#include <stdlib.h>

// Battery of tests
#include "test-data.h"

int main() {
    //uint8_t* a = data_gf256to12_inv_input[2];
    //uint8_t* b = data_gf256to12_inv_input[3];
    //uint8_t* c = data_gf256to12_inv_input[4];

    //log_cnsl(-1, v, 12);
    // uint8_t res[12];
    // gf2to96_pow256(res, v);
    // log_cnsl(0, res, 12);
    // gf2to96_pow256(res, res);
    // log_cnsl(1, res, 12);
    // gf2to96_pow256(res, res);
    // log_cnsl(2, res, 12);
    // gf2to96_pow256(res, res);
    // log_cnsl(3, res, 12);
    // gf2to96_pow256(res, res);
    // log_cnsl(4, res, 12);
    // gf2to96_pow256(res, res);
    // log_cnsl(5, res, 12);

    // uint8_t a[12], b[12], c[12];
    // for(int i=0; i<1000; i++) {
    //     for(int j=0; j<12; j++) {
    //         a[j] = rand() & 0xFF;
    //         b[j] = rand() & 0xFF;
    //         c[j] = rand() & 0xFF;
    //     }

    //     uint8_t res0[12], res1[12], res2[12];
    //     gf2to96_add(res0, b, c);
    //     gf2to96_mul(res0, res0, a);
    //     //log_cnsl(0, res, 12);

    //     gf2to96_mul(res1, a, b);
    //     gf2to96_mul(res2, a, c);
    //     gf2to96_add(res1, res1, res2);
    //     //log_cnsl(1, res, 12);

    //     if(!gf2to96_eq(res0, res1))
    //         printf("Alert!\n");
    // }

    // uint8_t a[12], b[12], c[12];
    // for(int i=0; i<1000; i++) {
    //     for(int j=0; j<12; j++) {
    //         a[j] = rand() & 0xFF;
    //         b[j] = rand() & 0xFF;
    //         c[j] = rand() & 0xFF;
    //     }

    //     uint8_t res0[12], res1[12];
    //     gf2to96_pow256(res0, a);
    //     //log_cnsl(0, res, 12);

    //     gf2to96_mul(res1, a, a);
    //     for(int j=0; j<256-2; j++)
    //         gf2to96_mul(res1, res1, a);
    //     //log_cnsl(1, res, 12);

    //     if(!gf2to96_eq(res0, res1))
    //         printf("Alert!\n");
    // }

    // uint8_t a[12], b[12], c[12];
    // for(int i=0; i<1000; i++) {
    //     for(int j=0; j<12; j++) {
    //         a[j] = rand() & 0xFF;
    //         b[j] = rand() & 0xFF;
    //         c[j] = rand() & 0xFF;
    //     }

    //     uint8_t res0[12];
    //     gf2to96_pow256(res0, a);
    //     gf2to96_pow256(res0, res0);
    //     gf2to96_pow256(res0, res0);
    //     gf2to96_pow256(res0, res0);

    //     if(!gf2to96_eq(res0, a))
    //         printf("Pas de Cycles!\n");
    // }

    uint8_t a[12];
    for(int k=0; k<100000; k++) {
        for(int j=0; j<12; j++) {
            a[j] = rand() & 0xFF;
        }
        uint8_t res0[12];
        gf2to96_pow256(res0, a);
        for(int i=0; i<12-1; i++) {
            if(gf2to96_eq(res0, a))
                printf("Cycles: %d!\n", 1+i);
            gf2to96_pow256(res0, res0);
        }
    }
}
