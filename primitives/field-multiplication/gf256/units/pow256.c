#include <stdio.h>
#include <string.h>

#include "gf256.h"
#define POWER 256
#define BYTESIZE 12
#define run_mul(z,x,y) gf2to96_mul(z,x,y)

int main() {
    for(int j=0; j<BYTESIZE; j++) {
        uint8_t e[BYTESIZE] = {0};
        e[j] = 1;
        uint8_t r[BYTESIZE];
        memcpy(r, e, BYTESIZE);
        for(int i=1; i<POWER; i++)
            run_mul(r, r, e);
        for(int i=0; i<BYTESIZE; i++)
            printf("%d.", r[i]);
        printf("\n");
    }

    return 0;
}
