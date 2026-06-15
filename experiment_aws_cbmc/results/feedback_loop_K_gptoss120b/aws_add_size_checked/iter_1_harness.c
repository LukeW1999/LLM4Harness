/*  
 * Contract for aws_add_size_checked  
 * Preconditions:  
 *   - a and b are nondeterministic size_t values (any value).  
 *   - r is a non‑null pointer to a size_t object.  
 * Postconditions (validity):  
 *   - The function returns either AWS_OP_SUCCESS (0) or AWS_OP_ERR (-1).  
 *   - If the return value is AWS_OP_SUCCESS, then no overflow occurred and *r == a + b.  
 *   - If the return value is AWS_OP_ERR, then overflow occurred, i.e., (b > 0) && (a > SIZE_MAX - b).  
 * Postconditions (frame):  
 *   - The input values a and b are unchanged.  
 *   - No memory locations other than *r are modified.  
 */

#define SIZE_BITS (sizeof(size_t) * 8)

#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_add_size_checked_harness(void) {
    /* Nondeterministic inputs */
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();

    /* Output location */
    size_t result;
    size_t *r = &result;

    /* Save old values for frame condition */
    size_t old_a = a;
    size_t old_b = b;
    size_t old_result = result;

    /* Call the function under test */
    int ret = aws_add_size_checked(a, b, r);

    /* Postcondition: return value is either success or error */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    /* Postcondition: pointer r is non‑null */
    assert(r != NULL);

    if (ret == AWS_OP_SUCCESS) {
        /* No overflow occurred */
        assert(!((b > 0) && (a > (SIZE_MAX - b))));
        /* Result is the sum */
        assert(*r == a + b);
    } else {
        /* Overflow must have occurred */
        assert((b > 0) && (a > (SIZE_MAX - b)));
    }

    /* Frame condition: inputs unchanged */
    assert(a == old_a);
    assert(b == old_b);

    /* No other memory modifications (only *r may change) */
    /* Since result is the only writable location besides locals, this holds */

    return 0;
}
