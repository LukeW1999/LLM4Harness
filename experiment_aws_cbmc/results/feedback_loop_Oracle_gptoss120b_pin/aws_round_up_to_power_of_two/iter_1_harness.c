#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* CBMC harness for aws_round_up_to_power_of_two */
void aws_round_up_to_power_of_two_harness(void) {
    /* Nondeterministic inputs */
    size_t n;               /* input value to round */
    size_t result;          /* output buffer */
    unsigned char pre[8];   /* memory before the output buffer */
    unsigned char post[8];  /* memory after the output buffer */

    /* Preserve original state for frame condition checks */
    size_t result_orig = result;
    unsigned char pre_copy[8];
    unsigned char post_copy[8];
    for (size_t i = 0; i < 8; ++i) {
        pre_copy[i] = pre[i];
        post_copy[i] = post[i];
    }

    /* Call the function under test */
    int ret = aws_round_up_to_power_of_two(n, &result);

    /* 1. Return value / error code correctness */
    if (n == 0) {
        /* n == 0 should succeed and set result to 1 */
        assert(ret == AWS_OP_SUCCESS);
        assert(result == 1);
    } else if (n > SIZE_MAX_POWER_OF_TWO) {
        /* n too large should fail with overflow error */
        assert(ret != AWS_OP_SUCCESS);
        /* result must remain unchanged on error */
        assert(result == result_orig);
    } else {
        /* Normal successful case */
        assert(ret == AWS_OP_SUCCESS);
        /* result must be a power of two */
        assert(aws_is_power_of_two(result));
        /* result must be >= original n */
        assert(result >= n);
        /* result must be the smallest such power of two */
        if (result > 1) {
            assert((result >> 1) < n);
        }
    }

    /* 2. Output buffer length/capacity invariants
       (No explicit length fields for a scalar, so nothing additional needed) */

    /* 3. Memory frame condition: only *result may be modified */
    for (size_t i = 0; i < 8; ++i) {
        assert(pre[i] == pre_copy[i]);
        assert(post[i] == post_copy[i]);
    }

    return 0;
}
