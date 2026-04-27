// === STEP 1: SUCCESS PATH ===
// aws_nospec_mask returns a size_t mask value (not AWS_OP_SUCCESS/ERR)
// When index < bound AND neither index nor bound has the sign bit set:
//   - Returns UINTPTR_MAX (all ones mask)
// When index >= bound OR index or bound has sign bit set:
//   - Returns 0 (zero mask)
//
// === STEP 2: FAILURE PATH ===
// No failure path in the traditional sense - this is a pure computation function
// It always returns either 0 or UINTPTR_MAX based on inputs
//
// === STEP 3: FRAME CONDITIONS ===
// No struct parameters - only two size_t values passed by value
// No side effects on any memory
//
// === STEP 4: VALIDITY INVARIANTS ===
// The function is a pure computation:
// - If index < bound AND (index | bound) does not have sign bit set:
//   result should be UINTPTR_MAX
// - Otherwise: result should be 0
// Key insight: negative_mask = index | bound; if either has sign bit, combined_mask has sign bit
// toobig_mask = bound - index - 1; if index >= bound, this wraps and has sign bit set
// combined_mask = negative_mask | toobig_mask
// If combined_mask has sign bit: ~combined_mask has sign bit clear, divided by top-bit-mask = 0
// If combined_mask has no sign bit: ~combined_mask has sign bit set, divided by top-bit-mask = 1
// Then multiplied by UINTPTR_MAX: 0*UINTPTR_MAX=0, 1*UINTPTR_MAX=UINTPTR_MAX

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <aws/common/byte_buf.h>

size_t aws_nospec_mask(size_t index, size_t bound);

void aws_nospec_mask_harness() {
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    size_t result = aws_nospec_mask(index, bound);

    // The mask should only be 0 or UINTPTR_MAX
    assert(result == 0 || result == UINTPTR_MAX);

    // Determine the "safe" condition:
    // Both index and bound must not have the sign bit set,
    // AND index must be strictly less than bound
    size_t sign_bit = SIZE_MAX - (SIZE_MAX >> 1);
    int index_has_sign = (index & sign_bit) != 0;
    int bound_has_sign = (bound & sign_bit) != 0;
    int either_has_sign = index_has_sign || bound_has_sign;

    if (!either_has_sign && index < bound) {
        // Safe range: result should be all-ones
        assert(result == UINTPTR_MAX);
    } else {
        // Out of range or sign bit set: result should be zero
        assert(result == 0);
    }

    // Additional cross-check using the same logic as the implementation
    size_t negative_mask = index | bound;
    size_t toobig_mask = bound - index - (uintptr_t)1;
    size_t combined_mask = negative_mask | toobig_mask;

    // If combined_mask has sign bit set, result should be 0
    if (combined_mask & sign_bit) {
        assert(result == 0);
    } else {
        assert(result == UINTPTR_MAX);
    }
}
