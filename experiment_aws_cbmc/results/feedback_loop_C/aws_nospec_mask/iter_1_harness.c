// === STEP 1: SUCCESS PATH ===
// When aws_nospec_mask returns a non-zero value:
//   - The return value is UINTPTR_MAX

// === STEP 2: FAILURE PATH ===
// When aws_nospec_mask returns 0:
//   - The return value is 0

// === STEP 3: FRAME CONDITIONS ===
// For the parameters of aws_nospec_mask, mark CHANGED or UNCHANGED:
//   index (size_t):
//     - UNCHANGED always
//   bound (size_t):
//     - UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - No specific validity invariants for primitive types size_t

#include <aws/common/private/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

void aws_nospec_mask_harness() {
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();
    size_t old_index = index;
    size_t old_bound = bound;

    size_t result = aws_nospec_mask(index, bound);

    // Assert frame conditions
    assert(index == old_index);
    assert(bound == old_bound);

    // Assert success path
    if (index < bound && bound <= (SIZE_MAX / 2) && index <= (SIZE_MAX / 2)) {
        assert(result == UINTPTR_MAX);
    } else {
        assert(result == 0);
    }
}
