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
    size_t r;
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    int result = aws_add_size_checked_varargs(num, &r, a, b);

    /* Postconditions:
     * 1. On success: r == a + b (no overflow occurred)
     * 2. On failure: overflow would have occurred
     * 3. Return value is either AWS_OP_SUCCESS (0) or AWS_OP_ERR (-1)
     */

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* No overflow: r contains the correct sum */
        assert(r == a + b);
        /* Verify no overflow actually occurred */
        assert(a + b >= a);
        assert(a + b >= b);
    } else {
        /* Overflow would have occurred */
        assert(result == AWS_OP_ERR);
        /* When overflow occurs, a + b wraps around, meaning a + b < a or a + b < b */
        /* We can verify the overflow condition held */
        assert(a > SIZE_MAX - b);
    }
}
