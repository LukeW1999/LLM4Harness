/*  
 * Contract for aws_mul_size_checked  
 * Preconditions:  
 *   - a and b are arbitrary size_t values.  
 *   - r is a non‑NULL pointer to a size_t object.  
 *   - The memory pointed to by r is initialized (any value).  
 * Postconditions (validity):  
 *   - The function returns either AWS_OP_SUCCESS (0) or AWS_OP_ERR (non‑zero).  
 *   - If the return value is AWS_OP_SUCCESS, then no overflow occurred and *r == a * b.  
 *   - If the return value is AWS_OP_ERR, then overflow occurred and *r is unchanged.  
 * Postconditions (frame):  
 *   - Only the object *r may be modified; all other memory remains unchanged.  
 */

#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_mul_size_checked_harness(void) {
    /* nondet inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* allocate result pointer */
    size_t *r = malloc(sizeof(size_t));
    __CPROVER_assume(r != NULL);
    /* initialize *r to an arbitrary value */
    *r = nondet_size_t();
    size_t old_r = *r;

    /* call the function under test */
    int result = aws_mul_size_checked(a, b, r);

    /* result must be either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* no overflow condition */
        if (a == 0) {
            assert(*r == 0);
        } else {
            assert(b <= SIZE_MAX / a);
            assert(*r == a * b);
        }
    } else {
        /* overflow must have occurred */
        if (a == 0) {
            /* multiplication by zero cannot overflow, so this path is unreachable */
            assert(false);
        } else {
            assert(b > SIZE_MAX / a);
        }
        /* *r must be unchanged */
        assert(*r == old_r);
    }

    free(r);
    return 0;
}
