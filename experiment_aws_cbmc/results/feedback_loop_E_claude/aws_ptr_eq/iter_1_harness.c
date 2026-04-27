#include <aws/common/hash_table.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_ptr_eq
 *
 * aws_ptr_eq compares two pointers for equality.
 * It returns true if a == b, false otherwise.
 *
 * Preconditions: none (any pointer values are valid)
 * Postconditions:
 *   - result is true iff a == b
 *   - result is false iff a != b
 */
void aws_ptr_eq_harness() {
    /* Non-deterministic pointer inputs */
    const void *a = (const void *)nondet_size_t();
    const void *b = (const void *)nondet_size_t();

    /* Call function under test */
    bool result = aws_ptr_eq(a, b);

    /* Assert postconditions */
    if (a == b) {
        assert(result == true);
    } else {
        assert(result == false);
    }

    /* Assert the result is consistent with pointer equality */
    assert(result == (a == b));
}
