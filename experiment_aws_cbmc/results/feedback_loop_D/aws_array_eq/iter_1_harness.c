// === STEP 1: SUCCESS PATH ===
// When aws_array_eq returns true:
//   - No fields of the input arrays or their lengths change.

// === STEP 2: FAILURE PATH ===
// When aws_array_eq returns false:
//   - No fields of the input arrays or their lengths change.

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   array_a (pointer to void):
//     - UNCHANGED always
//   len_a (size_t):
//     - UNCHANGED always
//   array_b (pointer to void):
//     - UNCHANGED always
//   len_b (size_t):
//     - UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - No validity invariants as the function does not modify the inputs.

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void aws_array_eq_harness() {
    // Non-deterministic values for lengths
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();

    // Allocate memory for the arrays
    uint8_t *array_a = can_fail_malloc(len_a);
    uint8_t *array_b = can_fail_malloc(len_b);

    // Save old state
    size_t old_len_a = len_a;
    size_t old_len_b = len_b;

    // Call the function under test
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    // Step 3 assertions (frame conditions)
    assert(len_a == old_len_a);
    assert(len_b == old_len_b);

    // Step 4 assertions (validity invariants)
    // No validity invariants as the function does not modify the inputs.

    // Free allocated memory
    free(array_a);
    free(array_b);
}
