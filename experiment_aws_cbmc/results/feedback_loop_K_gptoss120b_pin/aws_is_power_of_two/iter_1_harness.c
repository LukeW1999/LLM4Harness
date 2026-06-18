/* Contract:
   Preconditions:
     - none (x can be any size_t value)
   Postconditions (validity):
     - The returned boolean is true iff x != 0 and (x & (x - 1)) == 0.
   Postconditions (frame):
     - No memory locations are modified by aws_is_power_of_two. */

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <aws/common/math.h>

void aws_is_power_of_two_harness(void) {
    size_t x;
    /* x is nondeterministic */
    bool result = aws_is_power_of_two(x);

    /* Verify the specification */
    assert(result == (x != 0 && (x & (x - 1)) == 0));

    return 0;
}
