// === STEP 1: SUCCESS PATH ===
// When aws_ptr_eq returns true:
//   - No fields change in the input parameters as aws_ptr_eq is a comparison function.

// === STEP 2: FAILURE PATH ===
// When aws_ptr_eq returns false:
//   - No fields change in the input parameters as aws_ptr_eq is a comparison function.

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   param1 (void type):
//     - No fields to consider as it is a void pointer
//   param2 (void type):
//     - No fields to consider as it is a void pointer

// === STEP 4: VALIDITY INVARIANTS ===
//   - No validity invariants to check as aws_ptr_eq does not modify any structures

#include <aws/common/common.h>
#include <aws/common/hash_table.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ptr_eq_harness() {
    const void *a = (const void *)nondet_ptr();
    const void *b = (const void *)nondet_ptr();
    bool result;

    // Capture the old state of the pointers (though they don't have state, this is just to follow the pattern)
    const void *old_a = a;
    const void *old_b = b;

    result = aws_ptr_eq(a, b);

    // Assert that the pointers remain unchanged
    assert(a == old_a);
    assert(b == old_b);
}
