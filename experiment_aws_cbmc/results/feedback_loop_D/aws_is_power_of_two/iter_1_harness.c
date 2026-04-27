// === STEP 1: SUCCESS PATH ===
// When aws_round_up_to_power_of_two returns AWS_OP_SUCCESS (or the successful value):
//   - result: CHANGES to the smallest power of 2 >= n

// === STEP 2: FAILURE PATH ===
// When aws_round_up_to_power_of_two returns AWS_OP_ERR (or fails):
//   - result: UNCHANGED
//   (list every field, even if unchanged)

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   result (size_t type):
//     - result: CHANGED on success, UNCHANGED on failure

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_XXX_is_valid(&result): NO (since result is a primitive type, not a struct)

#include <aws/common/math.h>
#include <assert.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result;
    size_t old_result = result;

    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        // Success path assertions
        assert(result == (n > SIZE_MAX_POWER_OF_TWO ? old_result : ((size_t)1 << aws_clz_size(n == 0 ? 1 : n - 1))));
    } else {
        // Failure path assertions
        assert(result == old_result);
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }
}
