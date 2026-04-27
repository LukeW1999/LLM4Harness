// === STEP 1: SUCCESS PATH ===
// When aws_nospec_mask returns a non-zero mask (successful value):
//   - No fields are modified in the input parameters as the function does not take pointers to structs.
//   - The function returns a computed mask based on the input parameters.

// === STEP 2: FAILURE PATH ===
// When aws_nospec_mask returns a zero mask (failing value):
//   - No fields are modified in the input parameters as the function does not take pointers to structs.
//   - The function returns a computed mask based on the input parameters.

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   index (size_t):
//     - index: UNCHANGED always
//   bound (size_t):
//     - bound: UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - No validity invariants are checked as the function does not take pointers to structs.

#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_nospec_mask_harness() {
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    size_t old_index = index;
    size_t old_bound = bound;

    size_t result = aws_nospec_mask(index, bound);

    // Frame conditions
    assert(index == old_index);
    assert(bound == old_bound);

    // Validity invariants
    // No structs to validate
}
