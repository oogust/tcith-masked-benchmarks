#include <stdio.h>
#include "gf251.h"

int main() {
    printf(">> ADDITION\n");
    for(unsigned int i=0; i<256; i++)
        for(unsigned int j=0; j<256; j++)
            if(gf251_add(i, j) != ((i+j)%251))
                printf("ADD %d %d -> %d %d\n", i, j, gf251_add(i, j), (i+j)%251);

    printf(">> MULTIPLICATION\n");
    for(unsigned int i=0; i<256; i++)
        for(unsigned int j=0; j<256; j++)
            if(gf251_mul(i, j) != ((i*j)%251))
                printf("MUL %d %d -> %d %d\n", i, j, gf251_mul(i, j), (i*j)%251);

    printf(">> FIELD EXTENSION\n");
    uint64_t list_tests[5][3] = {
        {0x2D5A00016422F5FA, 0x2D5A00016422F5FA, 0x223FEFE1DF3C87D9},
        {0xA9DC85B0DAE218B5, 0x87EA93BF2F704C3F, 0x4DC5F801361DA938},
        {0xDDA155F9C39DCF54, 0xF476EC5C21EAEB68, 0x5833D1807CEB3ED2},
        {0xC34F08DE2B0E2196, 0xB531F64E473FB232, 0x5C12F45246434C17},
        {0xA5411F9A73C573F9, 0X6C989E0E76CB773C, 0xEA35F1A8D8BF21E3},
    };
    for(unsigned int i=0; i<5; i++) {
        uint8_t a[8], b[8], c[8], d[8];
        for(int j=0; j<8; j++) {
            a[j] = ((uint8_t*) &list_tests[i][0])[j];
            b[j] = ((uint8_t*) &list_tests[i][1])[j];
            c[j] = ((uint8_t*) &list_tests[i][2])[j];
        }
        //for(int j=0; j<8; j++)
        //    printf("(c) %d->%d\n", j, c[j]);
        gf251to8_mul(d, a, b);
        //for(int j=0; j<8; j++)
        //    printf("(d) %d->%d\n", j, d[j]);
        if(gf251to8_eq(c, d) != 1)
            printf("Error %d\n", i);
    }
}

