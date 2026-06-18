#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness(void) {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* bound the inputs to keep the state space reasonable */
    __CPROVER_assume(a <= 1000);
    __CPROVER_assume(b <= 1000);

    /* initialize result with a nondeterministic value */
    size_t old_result = nondet_size_t();
    size_t result = old_result;

    /* call the function under test */
    bool overflow = aws_mul_size_checked(a, b, &result);

    /* postconditions */
    if (!overflow) {
        /* when no overflow, result must be the product */
        assert(result == a * b);
    } else {
        /* when overflow, result must be unchanged */
        assert(result == old_result);
    }

    /* additional consistency checks based on the implementation contract */
    if (a == 0 || b == 0) {
        assert(!overflow);
        assert(result == 0);
    } else {
        if (a > SIZE_MAX / b) {
            assert(overflow);
        } else {
            assert(!overflow);
        }
    }
}
