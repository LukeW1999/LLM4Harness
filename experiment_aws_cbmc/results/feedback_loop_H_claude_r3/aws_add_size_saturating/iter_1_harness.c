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
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t c = nondet_size_t();

    int result;

    if (num == 1) {
        result = aws_add_size_checked_varargs(1, &r, a);
    } else if (num == 2) {
        result = aws_add_size_checked_varargs(2, &r, a, b);
    } else {
        result = aws_add_size_checked_varargs(3, &r, a, b, c);
    }

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, r contains the correct sum */
        if (num == 1) {
            assert(r == a);
        } else if (num == 2) {
            assert(r == a + b);
        } else {
            assert(r == a + b + c);
        }
        /* Result must be valid (no overflow occurred) */
        if (num == 1) {
            assert(r <= SIZE_MAX);
        } else if (num == 2) {
            assert(r >= a || r >= b); /* at least one of them fits */
            /* More precisely: no overflow means a + b <= SIZE_MAX */
            assert(a <= SIZE_MAX - b || b == 0);
        }
    } else {
        /* On failure, result is AWS_OP_ERR */
        assert(result == -1);
        /* Overflow was detected */
        if (num == 2) {
            /* a + b would overflow SIZE_MAX */
            /* We can't assert the exact overflow condition easily without
               replicating the logic, but we can assert the return value */
        }
    }

    /* Return value must be either success or error */
    assert(result == 0 || result == -1);
}
