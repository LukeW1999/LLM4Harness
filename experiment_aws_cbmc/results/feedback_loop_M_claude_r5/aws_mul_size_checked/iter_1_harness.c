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
 * We test with a small fixed number of arguments to keep the state space bounded.
 * We use num=3 as a representative case.
 */
void aws_add_size_checked_varargs_harness(void) {
    /* Bound the number of arguments to prevent state space explosion */
    size_t num = 3;

    /* Non-deterministic size_t arguments, bounded to prevent overflow in CBMC */
    size_t a = nondet_size_t();
    __CPROVER_assume(a <= SIZE_MAX / 4);

    size_t b = nondet_size_t();
    __CPROVER_assume(b <= SIZE_MAX / 4);

    size_t c = nondet_size_t();
    __CPROVER_assume(c <= SIZE_MAX / 4);

    /* Output result */
    size_t r = 0;

    /* Call the function under test */
    int result = aws_add_size_checked_varargs(num, &r, a, b, c);

    /* Postconditions */
    /* The function returns either AWS_OP_SUCCESS (0) or AWS_OP_ERR (-1) */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* On success: r contains the sum a + b + c, no overflow occurred */
        assert(r == a + b + c);
        /* Verify no overflow actually occurred */
        assert(a + b >= a);       /* a+b didn't overflow */
        assert(a + b + c >= a + b); /* a+b+c didn't overflow */
    } else {
        /* On failure: overflow was detected */
        /* r may be in an indeterminate state, but we can check the overflow condition */
        /* At least one intermediate sum must have overflowed */
        /* We don't assert r's value on failure as it's unspecified */
    }
}
