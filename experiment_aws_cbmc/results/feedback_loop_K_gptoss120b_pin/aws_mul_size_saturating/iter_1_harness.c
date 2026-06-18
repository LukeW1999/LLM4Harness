/*  
 * CONTRACT for aws_mul_size_saturating  
 * Preconditions:  
 *   - a and b are nondeterministic size_t values (no additional constraints).  
 * Postconditions (validity):  
 *   - The return value r is either the exact product a * b (when no overflow occurs)  
 *     or SIZE_MAX (when overflow occurs).  
 *   - No overflow detection side‑effects; r is a size_t value.  
 * Postconditions (frame):  
 *   - The function does not modify any memory locations (no side effects).  
 */

#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_mul_size_saturating_harness(void) {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* call the function under test */
    size_t r = aws_mul_size_saturating(a, b);

    /* compute expected result safely */
    bool overflow;
    size_t expected;

    if (a == 0 || b == 0) {
        overflow = false;
        expected = 0;
    } else {
        /* check for overflow: a > SIZE_MAX / b */
        overflow = (a > SIZE_MAX / b);
        if (overflow) {
            expected = SIZE_MAX;
        } else {
            expected = a * b;
        }
    }

    /* postcondition checks */
    assert(r == expected);
    assert(r == SIZE_MAX || r == a * b);
    return 0;
}
