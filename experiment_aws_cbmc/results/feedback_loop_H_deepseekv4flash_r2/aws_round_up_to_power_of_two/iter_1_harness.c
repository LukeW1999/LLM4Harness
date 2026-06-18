#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);
    size_t old_result = *result; // save for failure case

    int rval = aws_round_up_to_power_of_two(n, result);

    if (rval == AWS_OP_SUCCESS) {
        // On success, *result is the smallest power of two >= n
        // Check that *result is a power of two
        assert(*result != 0);
        assert((*result & (*result - 1)) == 0);
        // Check that *result >= n
        assert(*result >= n);
        // Check that *result is the smallest such power: *result/2 < n (unless n==0)
        if (n > 0) {
            assert(*result / 2 < n);
        } else {
            // n == 0 => *result == 1
            assert(*result == 1);
        }
    } else {
        // On failure, *result must be unchanged
        assert(*result == old_result);
        // Failure occurs only when n > SIZE_MAX_POWER_OF_TWO
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }
}
