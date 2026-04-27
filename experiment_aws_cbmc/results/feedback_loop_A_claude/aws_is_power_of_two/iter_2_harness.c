#include <aws/common/math.h>
#include <aws/common/common.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_is_power_of_two_harness(void) {
    size_t n;
    __CPROVER_assume(1); /* no constraints */

    bool result = aws_is_power_of_two(n);

    if (result) {
        /* If n is reported as a power of two, then n must be > 0 and (n & (n-1)) == 0 */
        assert(n > 0);
        assert((n & (n - 1)) == 0);
    } else {
        /* If n is not a power of two, then either n == 0 or (n & (n-1)) != 0 */
        assert(n == 0 || (n & (n - 1)) != 0);
    }
}
