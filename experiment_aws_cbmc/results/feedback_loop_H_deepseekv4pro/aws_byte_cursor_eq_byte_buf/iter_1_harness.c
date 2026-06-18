#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>

void aws_byte_cursor_eq_byte_buf_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_cursor a;
    struct aws_byte_buf b;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));

    ensure_byte_cursor_has_allocated_buffer_member(&a);
    ensure_byte_buf_has_allocated_buffer_member(&b);

    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* 2. Save old state before calling */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_buf old_b = b;

    /* Save a random byte from each buffer to check immutability */
    struct store_byte_from_buffer storage_a;
    struct store_byte_from_buffer storage_b;
    if (a.len > 0) {
        save_byte_from_array(a.ptr, a.len, &storage_a);
    }
    if (b.len > 0) {
        save_byte_from_array(b.buffer, b.len, &storage_b);
    }

    /* 3. Call function under test */
    bool rv = aws_byte_cursor_eq_byte_buf(&a, &b);

    /* 4. Postconditions */

    /* Structural invariants */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    /* Fields must be unchanged */
    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);
    assert(b.len == old_b.len);
    assert(b.buffer == old_b.buffer);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);

    /* Underlying bytes must not have been modified */
    if (a.len > 0) {
        assert_byte_from_buffer_matches(a.ptr, &storage_a);
    }
    if (b.len > 0) {
        assert_byte_from_buffer_matches(b.buffer, &storage_b);
    }

    /* Correctness: rv is true iff lengths are equal and all bytes match */
    bool expected = (a.len == b.len);
    if (expected) {
        /* lengths and MAX_BUFFER_SIZE guarantee bounded loop */
        for (size_t i = 0; i < a.len; ++i) {
            expected = expected && (a.ptr[i] == b.buffer[i]);
        }
    }
    assert(rv == expected);
}
