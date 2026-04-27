// === STEP 1: SUCCESS PATH ===
// When aws_round_up_to_power_of_two returns AWS_OP_SUCCESS:
//   - n == 0: *result = 1
//   - 0 < n <= SIZE_MAX_POWER_OF_TWO: *result = smallest power of 2 >= n
//   - return value = 0 (AWS_OP_SUCCESS)
//   - *result is a power of two
//   - *result >= n (original n before modification)
//
// === STEP 2: FAILURE PATH ===
// When aws_round_up_to_power_of_two returns AWS_OP_ERR:
//   - n > SIZE_MAX_POWER_OF_TWO
//   - *result: UNCHANGED (not written)
//   - return value = -1 (AWS_OP_ERR)
//
// === STEP 3: FRAME CONDITIONS ===
// n (size_t, passed by value): not modified by caller
// result (size_t*):
//   - *result: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
// On success:
//   - aws_is_power_of_two(*result) == true
//   - *result >= original_n
//   - return == AWS_OP_SUCCESS
// On failure:
//   - n > SIZE_MAX_POWER_OF_TWO
//   - *result unchanged
//   - return == AWS_OP_ERR

#include <aws/common/math.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

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
        // Return value must be AWS_OP_SUCCESS
        assert(ret == 0);

        // result must be a power of two
        assert(aws_is_power_of_two(result));

        // result must be >= original n
        assert(result >= n);

        // Special case: n == 0 => result == 1
        if (n == 0) {
            assert(result == 1);
        }

        // result must be the *smallest* power of two >= n:
        // i.e., result/2 < n (unless result == 1)
        if (result > 1) {
            assert((result >> 1) < n);
        }

        // n must have been <= SIZE_MAX_POWER_OF_TWO for success
        assert(n <= SIZE_MAX_POWER_OF_TWO);

    } else {
        // Return value must be AWS_OP_ERR
        assert(ret == -1);

        // n must have been > SIZE_MAX_POWER_OF_TWO
        assert(n > SIZE_MAX_POWER_OF_TWO);

        // result must be unchanged
        assert(result == old_result);
    }
}
