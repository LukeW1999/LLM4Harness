#include <aws/common/hash_table.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ptr_eq_harness() {
    /* aws_ptr_eq compares two pointers for equality.
     * Signature: bool aws_ptr_eq(const void *a, const void *b)
     * Returns true if a == b, false otherwise.
     * No preconditions required — pointers can be anything including NULL.
     * No state changes — pure function.
     */

    const void *a = (const void *)(uintptr_t)nondet_uint64_t();
    const void *b = (const void *)(uintptr_t)nondet_uint64_t();

    bool result = aws_ptr_eq(a, b);

    /* Postcondition: result is true iff a == b */
    if (a == b) {
        assert(result == true);
    } else {
        assert(result == false);
    }

    /* Equivalently, assert the logical equivalence */
    assert(result == (a == b));
}
