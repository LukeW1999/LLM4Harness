#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* The largest power of two that can be stored in a size_t */
#if SIZE_MAX == UINT32_MAX
#    define SIZE_BITS 32
#elif SIZE_MAX == UINT64_MAX
#    define SIZE_BITS 64
#endif

#define SIZE_MAX_POWER_OF_TWO (((size_t)1) << (SIZE_BITS - 1))

void aws_round_up_to_power_of_two_harness(void) {
    size_t n;
    size_t result;
    size_t result_before = result; /* capture pre-call value */

    int rc = aws_round_up_to_power_of_two(n, &result);

    /* Return value must be either AWS_OP_SUCCESS or AWS_OP_ERR */
    assert(rc == AWS_OP_SUCCESS || rc == AWS_OP_ERR);

    if (n == 0) {
        /* Special case: 0 rounds up to 1 */
        assert(rc == AWS_OP_SUCCESS);
        assert(result == 1);
    } else if (n > SIZE_MAX_POWER_OF_TWO) {
        /* Overflow case: must return error */
        assert(rc == AWS_OP_ERR);
        /* result must not be modified on error */
        assert(result == result_before);
    } else {
        /* Normal case: success */
        assert(rc == AWS_OP_SUCCESS);

        /* Result must be >= n */
        assert(result >= n);

        /* Result must be a power of two */
        assert(aws_is_power_of_two(result));

        /* Result must be the SMALLEST power of two >= n:
         * i.e., result/2 < n (unless result == 1) */
        if (result > 1) {
            assert((result >> 1) < n);
        }

        /* Result must not overflow: result <= SIZE_MAX_POWER_OF_TWO */
        assert(result <= SIZE_MAX_POWER_OF_TWO);
    }
}
