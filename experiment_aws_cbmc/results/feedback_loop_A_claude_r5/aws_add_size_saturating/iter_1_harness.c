#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_size_checked_varargs_harness(void) {
    /* aws_add_size_checked_varargs(size_t num, size_t *r, ...)
     * Adds [num] arguments (expected to be of size_t), and returns the result in *r.
     * If the result overflows, returns AWS_OP_ERR; otherwise returns AWS_OP_SUCCESS.
     */

    /* Use a small fixed number of arguments to keep the state space bounded */
    size_t num;
    __CPROVER_assume(num >= 1 && num <= 3);

    size_t r;
    size_t a, b, c;

    int result;

    if (num == 1) {
        result = aws_add_size_checked_varargs(1, &r, a);
    } else if (num == 2) {
        result = aws_add_size_checked_varargs(2, &r, a, b);
    } else {
        result = aws_add_size_checked_varargs(3, &r, a, b, c);
    }

    /* Postconditions */
    /* Return value must be either AWS_OP_SUCCESS (0) or AWS_OP_ERR (-1) */
    assert(result == 0 || result == -1);

    if (result == 0) {
        /* On success, r contains the sum of all arguments */
        if (num == 1) {
            assert(r == a);
        } else if (num == 2) {
            assert(r == a + b);
        } else {
            assert(r == a + b + c);
        }
    } else {
        /* On overflow/error, result is AWS_OP_ERR */
        assert(result == -1);
    }
}
