/*  
 * Contract for aws_round_up_to_power_of_two  
 * ------------------------------------------------------------  
 * Preconditions:  
 *   - n is an arbitrary size_t value (no explicit assumption).  
 *   - result points to a valid, writable size_t object (non‑NULL).  
 *   - The memory pointed to by result is initialized (old_result captured).  
 *   
 * Postconditions (validity):  
 *   - The function returns either AWS_OP_SUCCESS (0) or an error code (non‑zero).  
 *   - If the return value is AWS_OP_SUCCESS:  
 *       * *result is a power of two (aws_is_power_of_two(*result) == true).  
 *       * *result >= n (the rounded value is not smaller than the input).  
 *   - If the return value is an error:  
 *       * n > SIZE_MAX_POWER_OF_TWO (overflow condition).  
 *       * *result is unchanged (remains equal to old_result).  
 *   
 * Postconditions (frame):  
 *   - No memory locations other than *result are modified by the call.  
 */

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/common.h>
#include <aws/common/math.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = __CPROVER_nondet_size_t();

    /* allocate result pointer */
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);

    /* capture original value */
    size_t old_result = *result;

    /* call the function under test */
    int ret = aws_round_up_to_power_of_two(n, result);

    /* postcondition: return value is either success (0) or error (non‑zero) */
    assert(ret == AWS_OP_SUCCESS || ret != AWS_OP_SUCCESS);

    if (ret == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(aws_is_power_of_two(*result));

        /* result must be >= n (including the n == 0 case where result == 1) */
        assert(*result >= n);
    } else {
        /* error case occurs only when n is too large to round without overflow */
        assert(n > SIZE_MAX_POWER_OF_TWO);

        /* result must be unchanged */
        assert(*result == old_result);
    }

    /* clean up */
    free(result);
    return 0;
}
