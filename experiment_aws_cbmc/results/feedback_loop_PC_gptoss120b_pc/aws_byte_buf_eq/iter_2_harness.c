#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_eq_harness(void) {
    /* Declare and bound data structures */
    struct aws_byte_buf a;
    struct aws_byte_buf b;

    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));

    /* Ensure buffers are allocated if capacity > 0 */
    ensure_byte_buf_has_allocated_buffer_member(&a);
    ensure_byte_buf_has_allocated_buffer_member(&b);

    __CPROVER_assume(aws_byte_buf_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* Save old state */
    struct aws_byte_buf old_a = a;
    struct aws_byte_buf old_b = b;

    /* Call function under test */
    bool result = aws_byte_buf_eq(&a, &b);

    /* Compute expected equality */
    bool expected = false;
    if (a.len == b.len) {
        if (a.len == 0) {
            expected = true;
        } else if (a.buffer != NULL && b.buffer != NULL) {
            expected = (memcmp(a.buffer, b.buffer, a.len) == 0);
        }
    }

    /* Assert result matches expectation */
    assert(result == expected);

    /* Assert inputs are unchanged */
    assert(a.buffer   == old_a.buffer);
    assert(a.len      == old_a.len);
    assert(a.capacity == old_a.capacity);
    if (a.buffer != NULL && a.len > 0) {
        assert_bytes_match(a.buffer, old_a.buffer, a.len);
    }

    assert(b.buffer   == old_b.buffer);
    assert(b.len      == old_b.len);
    assert(b.capacity == old_b.capacity);
    if (b.buffer != NULL && b.len > 0) {
        assert_bytes_match(b.buffer, old_b.buffer, b.len);
    }

    /* Assert invariants always hold */
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));
}
