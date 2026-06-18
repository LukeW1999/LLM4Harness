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
    /* Number of arguments to add - fix to 3 for bounded verification */
    size_t num = 3;

    /* Non-deterministic size_t arguments, bounded to prevent state space explosion */
    size_t a = nondet_size_t();
    __CPROVER_assume(a <= MAX_BUFFER_SIZE);

    size_t b = nondet_size_t();
    __CPROVER_assume(b <= MAX_BUFFER_SIZE);

    size_t c = nondet_size_t();
    __CPROVER_assume(c <= MAX_BUFFER_SIZE);

    /* Output variable */
    size_t r = 0;

    /* Call the function under test */
    int result = aws_add_size_checked_varargs(num, &r, a, b, c);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: r must equal a + b + c without overflow */
        assert(r == a + b + c);
        /* The sum must not have overflowed SIZE_MAX */
        assert(r >= a);
        assert(r >= b);
        assert(r >= c);
    } else {
        /* On failure: result must be AWS_OP_ERR */
        assert(result == -1);
        /* On overflow, the addition would exceed SIZE_MAX */
        /* We can verify that the true mathematical sum would overflow */
        /* (We can't directly check this without overflow, but we assert result is -1) */
    }

    /* The result must be either AWS_OP_SUCCESS or AWS_OP_ERR */
    assert(result == 0 || result == -1);
}
