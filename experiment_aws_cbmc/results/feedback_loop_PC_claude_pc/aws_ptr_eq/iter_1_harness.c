#include <aws/common/hash_table.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * aws_ptr_eq: Equality function which compares pointer equality.
 * Returns true if a == b (as pointers), false otherwise.
 *
 * Analysis:
 * 1. Changed fields on success: N/A — pure function, no side effects.
 * 2. Unchanged fields: a and b are not modified.
 * 3. Failure path: N/A — always returns a bool, no error path.
 * 4. Validity invariants: result must equal (a == b).
 */

void aws_ptr_eq_harness(void) {
    /* 1. Declare inputs — two arbitrary const void * pointers */
    const void *a = (const void *)nondet_size_t();
    const void *b = (const void *)nondet_size_t();

    /* 2. Save old state (inputs must not change) */
    const void *old_a = a;
    const void *old_b = b;

    /* 3. Call function under test */
    bool result = aws_ptr_eq(a, b);

    /* 4. Assert postconditions */

    /* RETURN: result must be true iff a == b */
    if (a == b) {
        assert(result == true);
    } else {
        assert(result == false);
    }

    /* Equivalently, assert the exact expected value */
    assert(result == (a == b));

    /* FRAME: inputs must not have changed */
    assert(a == old_a);
    assert(b == old_b);
}
