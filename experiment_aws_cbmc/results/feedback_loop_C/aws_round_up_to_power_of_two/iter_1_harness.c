// === STEP 1: SUCCESS PATH ===
// When aws_round_up_to_power_of_two returns AWS_OP_SUCCESS (or the successful value):
//   - result: CHANGES to the smallest power of 2 >= n

// === STEP 2: FAILURE PATH ===
// When aws_round_up_to_power_of_two returns AWS_OP_ERR (or fails):
//   - result: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   result (size_t):
//     - result: CHANGED on success, UNCHANGED on failure

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_XXX_is_valid(&result): NO (result is a size_t, not a struct)

#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result;
    size_t old_result = result;

    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        // Success path assertions
        assert(result >= n);
        assert((result & (result - 1)) == 0); // result is a power of 2
    } else {
        // Failure path assertions
        assert(ret == AWS_OP_ERR);
        assert(result == old_result); // result should be unchanged
    }
}
