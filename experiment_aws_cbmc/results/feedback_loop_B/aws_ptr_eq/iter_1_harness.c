#include <aws/common/common.h>
#include <aws/common/hash_table.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_ptr_eq_harness() {
    /* 1. Declare and bound data structures */
    const void *a = (const void *)nondet_size_t();
    const void *b = (const void *)nondet_size_t();

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    /* No need to save state for primitive types */

    /* 3. Call function under test */
    bool result = aws_ptr_eq(a, b);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* For aws_ptr_eq, the result is simply the comparison of the pointers, so we assert the expected behavior */
    assert(result == (a == b));

    /* 5. Assert fields that must NOT change regardless of result */
    /* No fields to assert for primitive types */

    /* 6. Assert validity invariant always holds */
    /* No additional validity invariants for primitive types */
}
