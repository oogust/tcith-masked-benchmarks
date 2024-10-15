#include "gf251.h"
#include "test-include.h"

// Battery of tests
#include "test-data.h"

int main() {
    TEST_INIT;

    // Test Case 1: Test Addition
    TEST_CASE_BEGIN(gf251to12_add)
        uint8_t* v1 = data_gf251to12_add_input1[num_test];
        uint8_t* v2 = data_gf251to12_add_input2[num_test];
        uint8_t res[12];
        gf251to12_add(res, v1, v2);
        CHECK_EQ_SIZE(res, data_gf251to12_add_output[num_test], 12);
    TEST_CASE_END

    // Test Case 2: Test Substraction
    TEST_CASE_BEGIN(gf251to12_sub)
        uint8_t* v1 = data_gf251to12_sub_input1[num_test];
        uint8_t* v2 = data_gf251to12_sub_input2[num_test];
        uint8_t res[12];
        gf251to12_sub(res, v1, v2);
        CHECK_EQ_SIZE(res, data_gf251to12_sub_output[num_test], 12);
    TEST_CASE_END

    // Test Case 3: Test Multiplication
    TEST_CASE_BEGIN(gf251to12_mul)
        uint8_t* v1 = data_gf251to12_mul_input1[num_test];
        uint8_t* v2 = data_gf251to12_mul_input2[num_test];
        uint8_t res[12];
        gf251to12_mul(res, v1, v2);
        CHECK_EQ_SIZE(res, data_gf251to12_mul_output[num_test], 12);
    TEST_CASE_END

    // Test Case 4: Test Power 251
    TEST_CASE_BEGIN(gf251to12_pow251)
        uint8_t* v = data_gf251to12_pow251_input[num_test];
        uint8_t res[12];
        gf251to12_pow251(res, v);
        CHECK_EQ_SIZE(res, data_gf251to12_pow251_output[num_test], 12);
    TEST_CASE_END

    // Test Case 5: Test Inverse
    TEST_CASE_BEGIN(gf251to12_inv)
        uint8_t* v = data_gf251to12_inv_input[num_test];
        uint8_t res[12];
        gf251to12_inv(res, v);
        CHECK_EQ_SIZE(res, data_gf251to12_inv_output[num_test], 12);
    TEST_CASE_END

    // Test Case 6: Test Power251 GF(251^16)
    TEST_CASE_BEGIN(gf251to16_pow251)
        uint8_t* v = data_gf251to16_pow251_input[num_test];
        uint8_t res[16], res2[16];
        // Tested Function
        gf251to16_pow251(res, v);
        // Correct Output
        memcpy(res2, v, 16);
        for(int i=1; i<251; i++)
            gf251to16_mul(res2, res2, v);
        CHECK_EQ_SIZE(res, res2, 16);
    TEST_CASE_END

    // Test Case 7: Test Inverse GF(251^16)
    TEST_CASE_BEGIN(gf251to16_inv)
        uint8_t* v = data_gf251to16_inv_input[num_test];
        uint8_t res[16], check[16];
        // Tested Function
        gf251to16_inv(res, v);
        // Verification
        gf251to16_mul(check, res, v);
        const uint8_t one[16] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        CHECK_EQ_SIZE(check, one, 16);
    TEST_CASE_END

    return 0;
}
