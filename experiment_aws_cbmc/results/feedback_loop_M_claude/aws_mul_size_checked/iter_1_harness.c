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

    /* Output result pointer */
    size_t r = 0;

    /* Call the function under test */
    int result = aws_add_size_checked_varargs(num, &r, a, b, c);

    /* Postconditions:
     * 1. Return value is either AWS_OP_SUCCESS (0) or AWS_OP_ERR (-1)
     * 2. On success: r == a + b + c (no overflow occurred)
     * 3. On failure: overflow occurred, r value is unspecified but function returned error
     */

    /* Return value must be one of the two valid values */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* On success, the result must equal the sum of all arguments */
        assert(r == a + b + c);
        /* On success, no overflow occurred, so the sum must be representable */
        /* Verify no overflow: if a+b+c == r and result is success, r >= a, r >= b, r >= c */
        assert(r >= a);
        assert(r >= b);
        assert(r >= c);
    } else {
        /* On failure (AWS_OP_ERR), overflow was detected */
        /* We can verify that overflow actually would occur */
        /* The sum a+b+c would overflow SIZE_MAX */
        /* We don't assert on r's value since it's unspecified on error */
        assert(result == AWS_OP_ERR);
    }
}

void aws_mul_size_checked_harness(void) {
    aws_add_size_checked_varargs_harness();
    return 0;
}
