#include <aws/common/common.h>
#include <aws/common/math.h>
#include <aws/common/error.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* Nondeterministic input */
    size_t n = (size_t)nondet_uint64_t();

    /* Allocate result storage on the stack */
    size_t result_storage;
    size_t *result = &result_storage;
    *result = (size_t)nondet_uint64_t();

    /* Snapshot of inputs */
    size_t n_old = n;
    size_t result_old = *result;

    /* Call the function under test */
    int ret = aws_round_up_to_power_of_two(n, result);

    /* Post‑conditions */
    assert(n == n_old);                     /* input n must be unchanged */
    assert(result != NULL);                 /* result pointer must remain non‑NULL */

    /* return value must be one of the expected codes */
    assert(ret == AWS_OP_SUCCESS ||
           ret == AWS_ERROR_INVALID_ARGUMENT ||
           ret == AWS_ERROR_OVERFLOW_DETECTED);

    if (ret == AWS_OP_SUCCESS) {
        /* *result must be a power of two */
        assert((*result & (*result - 1)) == 0);
        /* and it must be at least the original n */
        assert(*result >= n_old);
    } else {
        /* on error the result must be unchanged (frame condition) */
        assert(*result == result_old);
    }
}
