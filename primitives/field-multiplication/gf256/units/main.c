#include "gf256.h"
#include "test-include.h"

// Battery of tests
#include "test-data.h"

int main() {
    TEST_INIT;

    // Test Case 1: Test Power256 GF(256^12)
    TEST_CASE_BEGIN(gf256to12_pow256)
        uint8_t* v = data_gf256to12_pow256_input[num_test];
        uint8_t res[12], res2[12];
        // Tested Function
        gf2to96_pow256(res, v);
        // Correct Output
        memcpy(res2, v, 12);
        for(int i=1; i<256; i++)
            gf2to96_mul(res2, res2, v);
        CHECK_EQ_SIZE(res, res2, 12);
    TEST_CASE_END

    // Test Case 2: Test Inverse GF(256^12)
    TEST_CASE_BEGIN(gf256to12_inv)
        uint8_t* v = data_gf256to12_inv_input[num_test];
        uint8_t res[12], check[12];
        // Tested Function
        gf2to96_inv(res, v);
        // Verification
        gf2to96_mul(check, res, v);
        const uint8_t one[12] = {1,0,0,0,0,0,0,0,0,0,0,0};
        CHECK_EQ_SIZE(check, one, 12);
    TEST_CASE_END

    // Test Case 3: Test Power256 GF(256^16)
    TEST_CASE_BEGIN(gf256to16_pow256)
        uint8_t* v = data_gf256to16_pow256_input[num_test];
        uint8_t res[16], res2[16];
        // Tested Function
        gf2to128_pow256(res, v);
        // Correct Output
        memcpy(res2, v, 16);
        for(int i=1; i<256; i++)
            gf2to128_mul(res2, res2, v);
        CHECK_EQ_SIZE(res, res2, 16);
    TEST_CASE_END

    // Test Case 4: Test Inverse GF(256^16)
    TEST_CASE_BEGIN(gf256to16_inv)
        uint8_t* v = data_gf256to16_inv_input[num_test];
        uint8_t res[16], check[16];
        // Tested Function
        gf2to128_inv(res, v);
        // Verification
        gf2to128_mul(check, res, v);
        const uint8_t one[16] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        CHECK_EQ_SIZE(check, one, 16);
    TEST_CASE_END

    return 0;
}
