#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * Harness for aws_add_size_checked_varargs:
 * "Adds [num] arguments (expected to be of size_t), and returns the result in *r.
 *  If the result overflows, returns AWS_OP_ERR; otherwise returns AWS_OP_SUCCESS."
 *
 * We test with a small fixed number of arguments to keep the state space bounded.
 * We use num=3 as a representative case.
 */
void aws_add_size_checked_varargs_harness(void) {
    /* Number of variadic arguments - use a small fixed value for bounded verification */
    size_t num = 3;

    /* Output pointer */
    size_t r;

    /* Non-deterministic size_t arguments */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t c = nondet_size_t();

    /* Call the function under test */
    int result = aws_add_size_checked_varargs(num, &r, a, b, c);

    /* Postconditions */
    /* Result must be either AWS_OP_SUCCESS (0) or AWS_OP_ERR (-1) */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* On success, r contains the sum a + b + c */
        assert(r == a + b + c);
        /* The sum must not have overflowed, so r >= each individual operand */
        assert(r >= a);
        assert(r >= b);
        assert(r >= c);
    } else {
        /* On failure (overflow), AWS_OP_ERR is returned */
        assert(result == AWS_OP_ERR);
        /* r is not guaranteed to be valid on overflow, so we don't assert its value */
    }
}
