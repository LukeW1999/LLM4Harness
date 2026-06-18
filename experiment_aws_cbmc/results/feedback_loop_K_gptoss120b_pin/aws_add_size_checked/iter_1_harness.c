/*=== Contract ===
Preconditions:
- __CPROVER_assume(r != NULL);
- __CPROVER_assume(a == __CPROVER_nondet_size_t());
- __CPROVER_assume(b == __CPROVER_nondet_size_t());

Postconditions (validity):
- The return value is either AWS_OP_SUCCESS (0) or AWS_OP_ERR (-1).
- If the return value is AWS_OP_SUCCESS, then *r == a + b and no overflow occurred.
- If the return value is AWS_OP_ERR, then overflow occurred (i.e., (b > 0) && (a > (SIZE_MAX - b))) and *r is unchanged.

Postconditions (frame):
- No memory locations other than *r (when success) are modified by the call.
===*/

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <aws/common/math.h>
#include <aws/common/common.h>

void aws_add_size_checked_harness(void) {
    size_t a = __CPROVER_nondet_size_t();
    size_t b = __CPROVER_nondet_size_t();

    size_t r_storage;
    size_t *r = &r_storage;
    __CPROVER_assume(r != NULL);

    /* Save original value of *r to check frame condition on error */
    size_t r_original = *r;

    int ret = aws_add_size_checked(a, b, r);

    /* Return value must be either success (0) or error (-1) */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    if (ret == AWS_OP_SUCCESS) {
        /* No overflow should have occurred */
        assert(!((b > 0) && (a > (SIZE_MAX - b))));
        /* Result must be the sum */
        assert(*r == a + b);
    } else {
        /* Overflow must have occurred */
        assert((b > 0) && (a > (SIZE_MAX - b)));
        /* *r must be unchanged */
        assert(*r == r_original);
    }

    return 0;
}
