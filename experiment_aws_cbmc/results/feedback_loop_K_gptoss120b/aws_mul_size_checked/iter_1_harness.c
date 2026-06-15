#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* CBMC harness for aws_mul_size_checked */
void aws_mul_size_checked_harness(void) {
    /* Allocate and initialize inputs */
    size_t a;
    size_t b;
    size_t *r = malloc(sizeof(size_t));
    assert(r != NULL);               /* allocation must succeed */

    /* Save original value of *r to check frame condition */
    size_t r_old = *r;

    /* No additional assumptions on a and b – they are nondeterministic */
    __CPROVER_assume(true);

    /* Call the function under test */
    int ret = aws_mul_size_checked(a, b, r);

    /* Postcondition: return value must be either success or error */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    if (ret == AWS_OP_SUCCESS) {
        /* No overflow must have occurred */
        assert(a == 0 || b <= SIZE_MAX / a);
        /* Result must be the exact product */
        assert(*r == a * b);
    } else {
        /* Overflow must have occurred */
        assert(a != 0 && b > SIZE_MAX / a);
        /* Frame condition: *r must be unchanged on error */
        assert(*r == r_old);
    }

    free(r);
    return 0;
}
