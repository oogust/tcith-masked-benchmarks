#include <stdio.h>
#include <string.h>

#include "gf251.h"
#define POWER 251
#define BYTESIZE 16
#define run_mul(z,x,y) gf251to16_mul(z,x,y)

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
