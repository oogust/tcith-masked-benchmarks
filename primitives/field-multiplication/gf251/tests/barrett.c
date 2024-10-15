#include <stdio.h>
#include <stdint.h>

uint64_t reduce_overflow64(uint64_t n, uint8_t k, uint64_t m, uint64_t a) {
    uint64_t q = (a*m)>>k;
    uint64_t res = a-n*q;
    return res;
}

int main() {
    uint64_t n = 31;

    for(uint8_t k=8; k<64; k++) {
        uint64_t m = ((1UL<<k) / n) + 1;
        uint64_t maxi = ((0xFFFFFFFFFFFFFFFF) / m);

        for(uint64_t a=0; a<=maxi; a++) {
            uint64_t r = reduce_overflow64(n, k, m, a);
            if(r != a % n) {
                printf("%d 1 %lld correctness %lld\n", k, a-1, m);
                break;
            } else if(a == maxi) {
                printf("%d 1 %lld overflow %lld\n", k, a, m);
                break;
            }
        }
    }

    return 0;
}
