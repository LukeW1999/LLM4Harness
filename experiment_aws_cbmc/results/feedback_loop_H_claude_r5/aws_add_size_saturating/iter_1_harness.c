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

    /* Use a small fixed number of arguments to keep the state space bounded */
    size_t num;
    __CPROVER_assume(num >= 1 && num <= 3);

    size_t r;
    int result;

    if (num == 1) {
        size_t a = nondet_size_t();
        result = aws_add_size_checked_varargs(1, &r, a);
        if (result == AWS_OP_SUCCESS) {
            assert(r == a);
        }
    } else if (num == 2) {
        size_t a = nondet_size_t();
        size_t b = nondet_size_t();
        result = aws_add_size_checked_varargs(2, &r, a, b);
        if (result == AWS_OP_SUCCESS) {
            /* No overflow: a + b fits in size_t */
            assert(r == a + b);
            /* Verify no overflow occurred */
            assert(r >= a);
            assert(r >= b);
        } else {
            /* Overflow detected */
            assert(result == AWS_OP_ERR);
        }
    } else {
        /* num == 3 */
        size_t a = nondet_size_t();
        size_t b = nondet_size_t();
        size_t c = nondet_size_t();
        result = aws_add_size_checked_varargs(3, &r, a, b, c);
        if (result == AWS_OP_SUCCESS) {
            assert(r == a + b + c);
            assert(r >= a);
            assert(r >= b);
            assert(r >= c);
        } else {
            assert(result == AWS_OP_ERR);
        }
    }

    /* Result is either AWS_OP_SUCCESS or AWS_OP_ERR */
    assert(result == 0 || result == -1);
}
