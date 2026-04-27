#include <aws/common/math.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

void aws_is_power_of_two_harness(void) {
    /* 1. Declare inputs */
    size_t n;

    /* n is unconstrained (all values valid as inputs) */

    /* 2. Call function under test */
    bool result = aws_is_power_of_two(n);

    /* 3. Assert postconditions */
    
    /* If n == 0, it is not a power of two */
    if (n == 0) {
        assert(!result);
    }
    
    /* If n == 1, it is a power of two (2^0) */
    if (n == 1) {
        assert(result);
    }
    
    /* If n is a power of two, then n & (n-1) == 0 */
    if (result) {
        assert(n != 0);
        assert((n & (n - 1)) == 0);
    }
    
    /* If n & (n-1) == 0 and n != 0, then it must be a power of two */
    if (n != 0 && (n & (n - 1)) == 0) {
        assert(result);
    }
    
    /* If n is not a power of two and n != 0, then n & (n-1) != 0 */
    if (!result && n != 0) {
        assert((n & (n - 1)) != 0);
    }
}
