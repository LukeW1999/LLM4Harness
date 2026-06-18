#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* 1. Non‑deterministic input bounded */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_BUFFER_SIZE);

    /* 2. Stack‑allocated output variable */
    size_t result_var;
    size_t *result = &result_var;
    *result = nondet_size_t();
    __CPROVER_assume(*result <= MAX_BUFFER_SIZE);
    size_t old_result = *result;

    /* 3. Call the function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* 4. Post‑condition checks for both success and failure paths */
    if (rc == AWS_OP_SUCCESS) {
        __CPROVER_assert(*result >= n, "result >= n");
        __CPROVER_assert(aws_is_power_of_two(*result), "result is power of two");
        if (n == 0) {
            __CPROVER_assert(*result == 1, "result == 1 when n == 0");
        } else {
            size_t half = *result >> 1;
            __CPROVER_assert(half < n, "half < n");
        }
    } else {
        __CPROVER_assert(*result == old_result, "result unchanged on failure");
    }

    /* 5. Return code must be one of the defined outcomes */
    __CPROVER_assert(rc == AWS_OP_SUCCESS || rc == AWS_OP_ERR, "rc is valid");
}
