#include <aws/common/hash_table.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * aws_ptr_eq: Equality function which compares pointer equality.
 * Returns true if a == b (pointer equality), false otherwise.
 *
 * Analysis:
 * 1. Changed fields on success: none (pure function, returns bool)
 * 2. Unchanged fields: none (no struct arguments)
 * 3. Failure: N/A (always returns a bool)
 * 4. Validity invariants: N/A (no struct arguments)
 *
 * Postconditions:
 * - result == (a == b)
 */
void aws_ptr_eq_harness(void) {
    /* Declare non-deterministic pointer inputs */
    const void *a = (const void *)(uintptr_t)nondet_uint64_t();
    const void *b = (const void *)(uintptr_t)nondet_uint64_t();

    /* Call function under test */
    bool result = aws_ptr_eq(a, b);

    /* Assert postcondition: result must equal pointer equality */
    assert(result == (a == b));

    /* Assert that if a == b, result is true */
    if (a == b) {
        assert(result == true);
    } else {
        assert(result == false);
    }
}
