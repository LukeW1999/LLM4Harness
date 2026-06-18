#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_round_up_to_power_of_two_harness() {
    /* 1. Declare and bound inputs */
    size_t n = nondet_size_t();               /* any size_t value */
    size_t r_storage = nondet_size_t();       /* any initial value */
    size_t *result = &r_storage;              /* must be non-null writable pointer */
    size_t old_result = r_storage;            /* save for immutability check on failure */

    /* 2. Call function under test */
    int ret = aws_round_up_to_power_of_two(n, result);

    /* 3. Postconditions */
    if (ret == AWS_OP_SUCCESS) {
        /* Success guarantees:
         *   *result is the smallest power of two >= n
         */
        assert(*result >= n);
        assert(aws_is_power_of_two(*result));

        if (n == 0) {
            assert(*result == 1);
        } else {
            /* Minimality check: half of *result must be < n (*result > 1 here) */
            assert((*result >> 1) < n);
        }
    } else {
        /* Failure: overflow would occur */
        assert(ret == AWS_OP_ERR);
        assert(*result == old_result);       /* *result must not have been modified */
        assert(n > SIZE_MAX_POWER_OF_TWO);   /* only condition that triggers error */
    }

    /* Global correctness: exactly one of success/error based on n */
    assert((n <= SIZE_MAX_POWER_OF_TWO) ? (ret == AWS_OP_SUCCESS) : (ret == AWS_OP_ERR));
}
