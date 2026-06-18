#include <aws/common/hash_table.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ptr_eq_harness() {
    /* aws_ptr_eq compares two pointers for equality.
     * It returns true if a == b, false otherwise.
     * The function does not dereference the pointers.
     * No state changes occur.
     */
    const void *a = (const void *)(uintptr_t)nondet_uint64_t();
    const void *b = (const void *)(uintptr_t)nondet_uint64_t();

    bool result = aws_ptr_eq(a, b);

    /* Postcondition: result is true iff a == b */
    assert(result == (a == b));

    /* Postcondition: if a == b, result must be true */
    if (a == b) {
        assert(result == true);
    } else {
        assert(result == false);
    }
}
