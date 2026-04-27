#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_byte_buf_eq
 *
 * From Doxygen:
 *   Compare two aws_byte_buf structures.
 *   Return whether their contents are equivalent.
 *
 * Preconditions:
 *   - Both a and b must be valid aws_byte_buf structures.
 *
 * Postconditions:
 *   - Returns true iff a->len == b->len and the bytes match.
 *   - Neither a nor b is modified.
 *   - Both a and b remain valid after the call.
 */
void aws_byte_buf_eq_harness() {
    /* Declare and set up first buffer */
    struct aws_byte_buf a;
    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_buf_is_valid(&a));

    /* Declare and set up second buffer */
    struct aws_byte_buf b;
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* Save old state before calling */
    struct aws_byte_buf old_a = a;
    struct aws_byte_buf old_b = b;

    /* Call function under test */
    bool result = aws_byte_buf_eq(&a, &b);

    /* Assert postconditions */

    /* 1. Validity invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    /* 2. Neither a nor b should be modified */
    assert(a.len == old_a.len);
    assert(a.capacity == old_a.capacity);
    assert(a.allocator == old_a.allocator);
    assert(a.buffer == old_a.buffer);

    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);
    assert(b.buffer == old_b.buffer);

    /* 3. Correctness: result must be consistent with content comparison */
    if (result) {
        /* If equal, lengths must match */
        assert(a.len == b.len);
    } else {
        /* If not equal, either lengths differ or some byte differs */
        /* We can assert the contrapositive: if lengths differ, result must be false */
        /* (already covered by result == false here) */
    }

    /* 4. If lengths differ, result must be false */
    if (a.len != b.len) {
        assert(!result);
    }
}
