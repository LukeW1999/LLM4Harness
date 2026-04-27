// === STEP 1: SUCCESS PATH ===
// When aws_ptr_eq returns true (or the successful value):
//   - The return value: true
//
// === STEP 2: FAILURE PATH ===
// When aws_ptr_eq returns false (or fails):
//   - The return value: false
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   param1 (const void *):
//     - The pointer itself: UNCHANGED always
//   param2 (const void *):
//     - The pointer itself: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_XXX_is_valid(&param1): N/A (not applicable, param1 is a void pointer)
//   - aws_XXX_is_valid(&param2): N/A (not applicable, param2 is a void pointer)
```

#include <aws/common/common.h>
#include <aws/common/hash_table.h>
#include <assert.h>

void aws_ptr_eq_harness() {
    const void *a = (const void *)nondet_ptr();
    const void *b = (const void *)nondet_ptr();

    // Save old state (not necessary for void pointers, but for completeness)
    const void *old_a = a;
    const void *old_b = b;

    bool result = aws_ptr_eq(a, b);

    // Frame conditions
    assert(a == old_a);
    assert(b == old_b);

    // Validity invariants (not applicable for void pointers)
}
