Looking at the function `aws_round_up_to_power_of_two`:

**Analysis:**
1. **What changes on success?** `*result` is set to the smallest power of 2 >= n. Returns `AWS_OP_SUCCESS`.
2. **What's unchanged?** `n` itself (input parameter, not modified). The output pointer `result` is only written on success.
3. **What happens on failure?** If `n > SIZE_MAX_POWER_OF_TWO`, returns `AWS_OP_ERR`. `*result` is NOT written.
4. **Validity invariants:** On success, `*result` must be a power of 2, `*result >= n`, and it must be the smallest such power of 2.

**Postconditions to assert:**
- On success: `*result` is a power of 2 (`aws_is_power_of_two(*result)`)
- On success: `*result >= n`
- On success: `*result / 2 < n` (smallest such power, unless result == 1)
- Special case: `n == 0` → `*result == 1`
- On failure: `n > SIZE_MAX_POWER_OF_TWO`
- Return value is either `AWS_OP_SUCCESS` or `AWS_OP_ERR`

```c
#include <aws/common/math.h>
#include <aws/common/common.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* 1. Declare inputs non-deterministically */
    size_t n;
    n = nondet_size_t();

    size_t result;

    /* 2. Call function under test */
    int ret = aws_round_up_to_power_of_two(n, &result);

    /* 3. Assert postconditions */

    /* Return value must be either success or error */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    if (ret == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(aws_is_power_of_two(result));

        /* result must be >= n */
        assert(result >= n);

        /* Special case: n == 0 => result == 1 */
        if (n == 0) {
            assert(result == 1);
        }

        /* result must be the SMALLEST power of 2 >= n:
         * If result > 1, then result/2 < n (otherwise result/2 would be a smaller valid answer).
         * Equivalently: result == 1 OR result/2 < n
         */
        assert(result == 1 || (result >> 1) < n);

        /* n must not have overflowed: n <= SIZE_MAX_POWER_OF_TWO */
        assert(n <= SIZE_MAX_POWER_OF_TWO);

        /* result must fit in size_t (no overflow) */
        assert(result <= SIZE_MAX);
        assert(result >= 1);
    } else {
        /* On failure, n must have been > SIZE_MAX_POWER_OF_TWO */
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }
}
