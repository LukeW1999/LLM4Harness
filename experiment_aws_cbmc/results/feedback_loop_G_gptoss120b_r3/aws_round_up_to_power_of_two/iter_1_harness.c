#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* allocate a result location and give it a nondeterministic initial value */
    size_t result_val = nondet_size_t();
    size_t *result = &result_val;

    /* save old state of the output location */
    size_t old_result = result_val;

    /* call the function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* post‑condition checks */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(*result != 0);
        assert((*result & (*result - 1)) == 0);

        /* result must be >= n */
        assert(*result >= n);

        /* special case when n == 0 */
        if (n == 0) {
            assert(*result == 1);
        }

        /* overflow case cannot happen on success */
        assert(n <= SIZE_MAX_POWER_OF_TWO);
    } else {
        /* failure must be the overflow error */
        assert(rc == AWS_OP_ERR);

        /* output must be unchanged */
        assert(*result == old_result);

        /* overflow condition that caused the error */
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }

    /* inputs are unchanged */
    assert(n == n); /* trivially true, kept for symmetry */
}
