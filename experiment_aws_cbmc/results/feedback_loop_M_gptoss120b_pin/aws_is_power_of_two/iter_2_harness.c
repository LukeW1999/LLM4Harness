#include <proof_helpers/make_common_data_structures.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input, bounded to keep state space finite */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= 1024);

    /* result buffer */
    size_t result = 0;
    size_t old_result = result;

    /* call the function under test */
    int ret = aws_round_up_to_power_of_two(n, &result);

    /* post‑condition checks */
    if (ret == AWS_OP_SUCCESS) {
        if (n == 0) {
            assert(result == 1);
        } else {
            /* result must be a power of two and >= n */
            assert(result >= n);
            assert(aws_is_power_of_two(result));
            /* minimality: the previous power of two is < n */
            assert((result >> 1) < n);
        }
    } else {
        /* result must be unchanged on failure */
        assert(result == old_result);
    }
}
