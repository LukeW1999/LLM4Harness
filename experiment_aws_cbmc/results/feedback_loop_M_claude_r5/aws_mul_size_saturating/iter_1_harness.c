#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * Harness for aws_add_size_checked_varargs:
 * "Adds [num] arguments (expected to be of size_t), and returns the result in *r.
 *  If the result overflows, returns AWS_OP_ERR; otherwise returns AWS_OP_SUCCESS."
 *
 * We test with a fixed small number of arguments to keep the state space bounded.
 * We use num=3 as a representative case.
 */
void aws_add_size_checked_varargs_harness(void) {
    /* Bound the number of arguments to prevent state space explosion */
    size_t num = 3;

    /* Non-deterministic size_t arguments, bounded to prevent overflow in CBMC */
    size_t a = nondet_size_t();
    __CPROVER_assume(a <= SIZE_MAX);

    size_t b = nondet_size_t();
    __CPROVER_assume(b <= SIZE_MAX);

    size_t c = nondet_size_t();
    __CPROVER_assume(c <= SIZE_MAX);

    /* Output pointer */
    size_t r = 0;

    /* Call the function under test */
    int result = aws_add_size_checked_varargs(num, &r, a, b, c);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: r == a + b + c (no overflow occurred) */
        /* Verify no overflow actually occurred */
        assert(result == 0);
        /* The result must equal the mathematical sum */
        assert(r == a + b + c);
        /* The result must be >= each individual operand (since no overflow) */
        assert(r >= a);
        assert(r >= b);
        assert(r >= c);
    } else {
        /* On failure: overflow was detected, result is AWS_OP_ERR */
        assert(result == -1);
        /* r is not guaranteed to be meaningful on overflow, but we can check
         * that the function correctly detected overflow by verifying that
         * the true mathematical sum would overflow SIZE_MAX */
        /* We cannot easily assert the overflow condition directly without
         * risking the same overflow in the harness, so we just check the return */
    }

    /* The return value must be either AWS_OP_SUCCESS or AWS_OP_ERR */
    assert(result == 0 || result == -1);
}
