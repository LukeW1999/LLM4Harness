#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result;

    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        /* Postcondition: result is a power of 2 */
        assert(aws_is_power_of_two(result));
        
        /* Postcondition: result is >= n */
        assert(result >= n);
        
        /* Postcondition: result is the SMALLEST power of 2 >= n.
         * If result > 1, then the next smaller power of 2 (result >> 1) 
         * must be strictly less than n. */
        if (result > 1) {
            assert((result >> 1) < n);
        }
    } else {
        /* Postcondition: failure only occurs when n exceeds the largest 
         * representable power of 2 for size_t (overflow condition). */
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }
}
