#include <aws/common/hash_table.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * aws_ptr_eq: Equality function which compares pointer equality.
 * Returns true if a == b (pointer comparison), false otherwise.
 *
 * Analysis:
 * 1. Changed fields on success: none (pure function, returns bool)
 * 2. Unchanged fields: none (no struct inputs)
 * 3. Failure: N/A (always returns a bool)
 * 4. Validity invariants: result must equal (a == b)
 */

void aws_ptr_eq_harness(void) {
    /* Non-deterministic pointer inputs */
    const void *a = (const void *)nondet_size_t();
    const void *b = (const void *)nondet_size_t();

    /* Call the function under test */
    bool result = aws_ptr_eq(a, b);

    /* Postconditions:
     * The function returns true iff a == b (pointer equality).
     */
    if (a == b) {
        assert(result == true);
    } else {
        assert(result == false);
    }

    /* Equivalently, assert the exact relationship */
    assert(result == (a == b));
}
