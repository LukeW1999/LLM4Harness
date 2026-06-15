#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n;               /* CBMC treats uninitialized locals as nondet */
    size_t result;          /* output variable */

    /* Frame condition: allocate a buffer that must remain unchanged */
    size_t *buf = malloc(10 * sizeof(size_t));
    __CPROVER_assume(buf != NULL);
    size_t buf_snapshot[10];
    for (size_t i = 0; i < 10; ++i) {
        buf[i] = (size_t)0;               /* any nondet value is fine */
        buf_snapshot[i] = buf[i];
    }

    /* Call the function under test */
    int ret = aws_round_up_to_power_of_two(n, &result);

    /* 1. Return value / error code correctness */
    if (ret == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(aws_is_power_of_two(result));
        /* result must be >= n */
        assert(result >= n);
        /* result must be the smallest such power of two */
        if (result > 1) {
            assert((result >> 1) < n);
        }
    } else {
        /* The only error condition is overflow */
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }

    /* 2. Output buffer length/capacity invariants */
    /* (no explicit length invariants needed beyond the above checks) */

    /* 3. Frame condition: ensure unrelated memory is untouched */
    for (size_t i = 0; i < 10; ++i) {
        assert(buf[i] == buf_snapshot[i]);
    }

    free(buf);
    return 0;
}
