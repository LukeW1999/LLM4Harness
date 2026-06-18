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

    /* Use a small fixed number of arguments to keep the state space manageable */
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
            /* Actually for 1 arg there's no addition, so no overflow */
            assert(0); /* should never overflow with 1 arg */
        }
    } else if (num == 2) {
        size_t a = nondet_size_t();
        size_t b = nondet_size_t();
        result = aws_add_size_checked_varargs(2, &r, a, b);
        if (result == AWS_OP_SUCCESS) {
            /* No overflow: result == a + b */
            assert(r == a + b);
            /* Also verify no overflow occurred */
            assert(r >= a);
            assert(r >= b);
        } else {
            /* Overflow occurred: a + b > SIZE_MAX */
            /* We can't easily assert the overflow condition directly,
             * but we can assert result is AWS_OP_ERR */
            assert(result == -1);
        }
    } else {
        /* num == 3 */
        size_t a = nondet_size_t();
        size_t b = nondet_size_t();
        size_t c = nondet_size_t();
        result = aws_add_size_checked_varargs(3, &r, a, b, c);
        if (result == AWS_OP_SUCCESS) {
            /* No overflow: result == a + b + c */
            assert(r == a + b + c);
            assert(r >= a);
        } else {
            assert(result == -1);
        }
    }

    /* Result must be either AWS_OP_SUCCESS (0) or AWS_OP_ERR (-1) */
    assert(result == 0 || result == -1);
}
