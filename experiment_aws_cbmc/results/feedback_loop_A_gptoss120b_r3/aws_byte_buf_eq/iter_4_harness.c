#include <assert.h>
#include <stdbool.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_eq_harness(void) {
    struct aws_byte_buf a;
    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_buf_is_valid(&a));

    struct aws_byte_buf b;
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    struct aws_byte_buf old_a = a;
    struct aws_byte_buf old_b = b;

    struct store_byte_from_buffer a_store, b_store;
    if (a.buffer != NULL && a.len > 0) {
        save_byte_from_array(a.buffer, a.len, &a_store);
    }
    if (b.buffer != NULL && b.len > 0) {
        save_byte_from_array(b.buffer, b.len, &b_store);
    }

    bool result = aws_byte_buf_eq(&a, &b);

    /* Buffers remain valid and unchanged */
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    assert(a.allocator == old_a.allocator);
    assert(a.capacity == old_a.capacity);
    assert(a.len == old_a.len);
    assert(a.buffer == old_a.buffer);
    if (a.buffer != NULL && a.len > 0) {
        assert_byte_from_buffer_matches(a.buffer, &a_store);
    }

    assert(b.allocator == old_b.allocator);
    assert(b.capacity == old_b.capacity);
    assert(b.len == old_b.len);
    assert(b.buffer == old_b.buffer);
    if (b.buffer != NULL && b.len > 0) {
        assert_byte_from_buffer_matches(b.buffer, &b_store);
    }

    /* Result consistency */
    if (result) {
        assert(a.len == b.len);
        if (a.len > 0) {
            assert_bytes_match(a.buffer, b.buffer, a.len);
        }
    }
}
