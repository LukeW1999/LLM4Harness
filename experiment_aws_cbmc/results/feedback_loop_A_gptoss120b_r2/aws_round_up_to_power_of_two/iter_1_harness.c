#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

/* AWS operation result codes */
#ifndef AWS_OP_SUCCESS
#   define AWS_OP_SUCCESS 0
#endif
#ifndef AWS_OP_ERR
#   define AWS_OP_ERR    -1
#endif

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* save original input for post‑condition checks */
    size_t old_n = n;

    /* nondeterministic initial value for the output location */
    size_t result = nondet_size_t();
    size_t old_result = result;

    /* call the function under test */
    int ret = aws_round_up_to_power_of_two(n, &result);

    /* post‑conditions for the success path */
    if (ret == AWS_OP_SUCCESS) {
        if (old_n == 0) {
            /* n == 0 ⇒ result must be 1 */
            assert(result == 1);
        } else {
            /* n > 0 and n ≤ SIZE_MAX_POWER_OF_TWO ⇒ result is a power of two
               and result ≥ original n */
            assert(aws_is_power_of_two(result));
            assert(result >= old_n);
        }
    } else {
        /* failure path: overflow detected, result must be unchanged */
        assert(ret == AWS_OP_ERR);
        assert(result == old_result);
        assert(old_n > SIZE_MAX_POWER_OF_TWO);
    }

    /* no other mutable state, so no further unchanged‑field asserts needed */
}
