// === STEP 1: SUCCESS PATH ===
// When aws_round_up_to_power_of_two returns AWS_OP_SUCCESS:
//   - result: CHANGES to the smallest power of 2 >= n
//   - Specifically: if n == 0, result = 1; otherwise result is smallest power of 2 >= n
//   - The returned result must be a power of two
//   - The returned result must be >= n
//
// === STEP 2: FAILURE PATH ===
// When aws_round_up_to_power_of_two returns AWS_OP_ERR:
//   - result: UNCHANGED (not written)
//   - This happens when n > SIZE_MAX_POWER_OF_TWO
//
// === STEP 3: FRAME CONDITIONS ===
// n (size_t, input): UNCHANGED always (passed by value)
// result (size_t*, output):
//   - *result: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
// On success:
//   - *result is a power of two (aws_is_power_of_two(*result) == true)
//   - *result >= n
//   - *result <= SIZE_MAX (trivially true for size_t)
// On failure:
//   - return value is AWS_OP_ERR (-1)
//   - *result is unchanged

#include <aws/common/math.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

void aws_round_up_to_power_of_two_harness(void) {
    size_t n;
    size_t result;
    size_t old_result;

    // Nondeterministic inputs
    n = nondet_size_t();
    result = nondet_size_t();
    old_result = result;

    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        // Result must be a power of two
        assert(aws_is_power_of_two(result));

        // Result must be >= n
        assert(result >= n);

        // If n == 0, result must be 1
        if (n == 0) {
            assert(result == 1);
        }

        // Result must be the smallest power of two >= n
        // i.e., result/2 < n (unless result == 1)
        if (result > 1) {
            assert((result >> 1) < n);
        }

        // n must have been <= SIZE_MAX_POWER_OF_TWO for success
        assert(n <= SIZE_MAX_POWER_OF_TWO);

    } else {
        // Return value must be AWS_OP_ERR
        assert(ret == AWS_OP_ERR);

        // result must be unchanged
        assert(result == old_result);

        // n must have been > SIZE_MAX_POWER_OF_TWO for failure
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }
}
