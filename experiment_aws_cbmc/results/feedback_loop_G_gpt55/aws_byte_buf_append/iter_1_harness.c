#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_append_harness(void) {
    struct aws_byte_buf to;
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    to.allocator = aws_default_allocator();
    ensure_byte_buf_has_allocated_buffer_member(&to);
    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(to.capacity <= MAX_BUFFER_SIZE);
    if (to.capacity > 0) {
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(to.buffer, to.capacity));
    }

    struct aws_byte_cursor from;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&from);

    if (nondet_bool()) {
        from.len = to.len;
        from.ptr = to.buffer;
    }

    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));
    __CPROVER_assume(from.len <= MAX_BUFFER_SIZE);
    if (from.len > 0) {
        __CPROVER_assume(AWS_MEM_IS_READABLE(from.ptr, from.len));
    }

    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    uint8_t old_to_buffer[MAX_BUFFER_SIZE];
    uint8_t old_from_buffer[MAX_BUFFER_SIZE];

    if (old_to.capacity > 0) {
        __builtin_memcpy(old_to_buffer, old_to.buffer, old_to.capacity);
    }

    if (old_from.len > 0) {
        __builtin_memcpy(old_from_buffer, old_from.ptr, old_from.len);
    }

    int result = aws_byte_buf_append(&to, &from);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (old_to.capacity - old_to.len < old_from.len) {
        assert(result == AWS_OP_ERR);
    } else {
        assert(result == AWS_OP_SUCCESS);
    }

    if (result == AWS_OP_SUCCESS) {
        size_t append_end = old_to.len + old_from.len;

        assert(old_to.capacity - old_to.len >= old_from.len);
        assert(to.len == append_end);

        if (old_to.len > 0) {
            assert_bytes_match(to.buffer, old_to_buffer, old_to.len);
        }

        if (old_from.len > 0) {
            assert_bytes_match(to.buffer + old_to.len, old_from_buffer, old_from.len);
        }

        if (old_to.capacity > append_end) {
            assert_bytes_match(
                to.buffer + append_end,
                old_to_buffer + append_end,
                old_to.capacity - append_end);
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(old_to.capacity - old_to.len < old_from.len);
        assert(to.len == old_to.len);

        if (old_to.capacity > 0) {
            assert_bytes_match(to.buffer, old_to_buffer, old_to.capacity);
        }
    }

    assert(to.buffer == old_to.buffer);
    assert(to.capacity == old_to.capacity);
    assert(to.allocator == old_to.allocator);

    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    if (old_from.len > 0) {
        assert_bytes_match(from.ptr, old_from_buffer, old_from.len);
    }

    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
