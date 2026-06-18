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

    /* Use a fixed small number of arguments to keep the state space bounded.
     * We'll test with num=2 as a representative case. */
    size_t num = 2;
    size_t a;
    size_t b;
    size_t r;

    /* Call the function with 2 size_t arguments */
    int result = aws_add_size_checked_varargs(num, &r, a, b);

    /* Postconditions:
     * 1. On success: r == a + b (no overflow occurred)
     * 2. On failure: overflow would have occurred (a + b > SIZE_MAX)
     * 3. Return value is either AWS_OP_SUCCESS (0) or AWS_OP_ERR (-1)
     */

    /* Return value must be either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* On success, r contains the correct sum */
        assert(r == a + b);
        /* No overflow: a + b <= SIZE_MAX */
        assert(a <= SIZE_MAX - b);
    } else {
        /* On failure (overflow), result is AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
        /* Overflow occurred: a + b > SIZE_MAX */
        assert(a > SIZE_MAX - b);
    }
}
