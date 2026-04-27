// === STEP 1: SUCCESS PATH ===
// When aws_is_power_of_two returns true (or the successful value):
//   - n: UNCHANGED
//   - result: CHANGES to the smallest power of 2 greater than or equal to n

// === STEP 2: FAILURE PATH ===
// When aws_is_power_of_two returns false (or fails):
//   - n: UNCHANGED
//   - result: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY parameter, mark CHANGED or UNCHANGED:
//   n (size_t):
//     - UNCHANGED always
//   result (size_t*):
//     - CHANGED on success, UNCHANGED on failure

// === STEP 4: VALIDITY INVARIANTS ===
//   - No validity invariants specified for size_t parameters

#include <aws/common/math.h>
#include <assert.h>

void aws_is_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result;
    size_t old_result = result;

    bool ret = aws_is_power_of_two(n);

    // Step 1: Success path assertions
    if (ret) {
        assert(result >= n);
        assert((result & (result - 1)) == 0); // result is a power of 2
    } else {
        // Step 2: Failure path assertions
        assert(result == old_result);
    }

    // Step 3: Frame conditions
    assert(n == n); // n is unchanged
    if (ret) {
        assert(result != old_result); // result is changed on success
    } else {
        assert(result == old_result); // result is unchanged on failure
    }
}
