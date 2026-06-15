#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic inputs */
    size_t n = nondet_size_t();
    size_t result_val = nondet_size_t();
    size_t *result = &result_val;

    /* save old state of the output location */
    size_t old_result = result_val;

    /* result pointer must be non‑NULL and writable */
    __CPROVER_assume(result != NULL);

    /* call the function under test */
    int ret = aws_round_up_to_power_of_two(n, result);

    /* post‑conditions for both success and failure paths */
    if (ret == AWS_OP_SUCCESS) {
        /* result is a power of two */
        assert((*result != 0) && ((*result & (*result - 1)) == 0));

        /* result is the smallest power of two >= n */
        assert(*result >= n);
        if (n == 0) {
            assert(*result == 1);
        } else {
            /* the previous power of two is strictly less than n */
            assert((*result >> 1) < n);
        }
    } else {
        /* on failure the output must be unchanged */
        assert(*result == old_result);
    }
}
