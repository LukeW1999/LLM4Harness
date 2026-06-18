#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_size_checked_varargs_harness(void) {
    /* aws_add_size_checked_varargs: Adds [num] arguments (expected to be of size_t),
     * and returns the result in *r.
     * If the result overflows, returns AWS_OP_ERR; otherwise returns AWS_OP_SUCCESS.
     */

    /* Use a small fixed num to keep state space manageable */
    size_t num;
    __CPROVER_assume(num >= 1 && num <= 3);

    size_t r;
    int result;

    if (num == 1) {
        size_t a = nondet_size_t();
        result = aws_add_size_checked_varargs(1, &r, a);
        if (result == AWS_OP_SUCCESS) {
            assert(r == a);
        } else {
            /* overflow: for 1 arg, no overflow possible */
            assert(0); /* single arg can't overflow */
        }
    } else if (num == 2) {
        size_t a = nondet_size_t();
        size_t b = nondet_size_t();
        result = aws_add_size_checked_varargs(2, &r, a, b);
        if (result == AWS_OP_SUCCESS) {
            /* No overflow: result should equal a + b */
            assert(r == a + b);
            /* Verify no overflow occurred */
            assert(r >= a);
            assert(r >= b);
        } else {
            /* Overflow occurred: a + b > SIZE_MAX */
            assert(result == AWS_OP_ERR);
            /* When overflow, r is not guaranteed to be set correctly */
        }
    } else {
        /* num == 3 */
        size_t a = nondet_size_t();
        size_t b = nondet_size_t();
        size_t c = nondet_size_t();
        result = aws_add_size_checked_varargs(3, &r, a, b, c);
        if (result == AWS_OP_SUCCESS) {
            /* No overflow: result should equal a + b + c */
            assert(r == a + b + c);
            assert(r >= a);
            assert(r >= b);
            assert(r >= c);
        } else {
            assert(result == AWS_OP_ERR);
        }
    }

    /* Return value must be either AWS_OP_SUCCESS or AWS_OP_ERR */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
}
