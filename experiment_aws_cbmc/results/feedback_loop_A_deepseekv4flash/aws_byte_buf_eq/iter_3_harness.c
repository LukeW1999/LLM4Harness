#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>

void aws_byte_buf_eq_harness() {
    /* Non-deterministic flag: whether buffers should be equal */
    bool equal = nondet_bool();

    /* Create two byte buffers */
    struct aws_byte_buf a, b;

    /* Bounding and validity preconditions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&a);
    ensure_byte_buf_has_allocated_buffer_member(&b);
    /* Ensure buffer pointers are non-null when capacity > 0 */
    __CPROVER_assume(a.buffer != NULL || a.capacity == 0);
    __CPROVER_assume(b.buffer != NULL || b.capacity == 0);
    __CPROVER_assume(aws_byte_buf_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* Save old state of a (immutable) */
    struct aws_byte_buf old_a = a;
    /* Save current state of b, before any modifications for the "equal" branch */
    struct aws_byte_buf b_modified;

    /* Condition buffers according to equality flag */
    if (equal) {
        /* Make buffers identical */
        b = a;
        b_modified = b;
    } else {
        /* Force buffers to be different by making lengths unequal.
         * Ensure capacity allows the new lengths. */
        __CPROVER_assume(a.len != b.len);
        b_modified = b;
    }

    /* The buffers we will call the function with */
    struct aws_byte_buf a_call = a;
    struct aws_byte_buf b_call = b;

    /* Call the function under test */
    bool result = aws_byte_buf_eq(&a_call, &b_call);

    /* Postconditions: result matches expected equality */
    if (equal) {
        assert(result == true);
    } else {
        assert(result == false);
    }

    /* Immutability of a (original) */
    assert(aws_byte_buf_is_valid(&a_call));
    assert(a_call.len == old_a.len);
    assert(a_call.capacity == old_a.capacity);
    assert(a_call.allocator == old_a.allocator);
    assert(a_call.buffer == old_a.buffer);
    assert_bytes_match(a_call.buffer, old_a.buffer, a_call.len);

    /* Immutability of b (with respect to its state before the call) */
    assert(aws_byte_buf_is_valid(&b_call));
    assert(b_call.len == b_modified.len);
    assert(b_call.capacity == b_modified.capacity);
    assert(b_call.allocator == b_modified.allocator);
    assert(b_call.buffer == b_modified.buffer);
    assert_bytes_match(b_call.buffer, b_modified.buffer, b_call.len);
}
