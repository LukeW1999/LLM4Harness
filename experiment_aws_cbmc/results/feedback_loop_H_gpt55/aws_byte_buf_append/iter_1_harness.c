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

    to.len = nondet_size_t();
    to.capacity = nondet_size_t();
    to.allocator = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&to);
    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(to.capacity == 0 || AWS_MEM_IS_WRITABLE(to.buffer, to.capacity));

    struct aws_byte_cursor from;

    if (nondet_bool()) {
        from.len = to.len;
        from.ptr = to.buffer;
    } else {
        from.len = nondet_size_t();
        __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
        ensure_byte_cursor_has_allocated_buffer_member(&from);
    }

    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));
    __CPROVER_assume(from.len == 0 || AWS_MEM_IS_READABLE(from.ptr, from.len));

    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    struct store_byte_from_buffer old_to_byte;
    if (old_to.capacity > 0) {
        save_byte_from_array(old_to.buffer, old_to.capacity, &old_to_byte);
    }

    struct store_byte_from_buffer old_to_prefix_byte;
    if (old_to.len > 0) {
        save_byte_from_array(old_to.buffer, old_to.len, &old_to_prefix_byte);
    }

    struct store_byte_from_buffer old_from_byte;
    if (old_from.len > 0) {
        save_byte_from_array(old_from.ptr, old_from.len, &old_from_byte);
    }

    int result = aws_byte_buf_append(&to, &from);

    if (old_to.capacity - old_to.len < old_from.len) {
        assert(result == AWS_OP_ERR);

        assert(to.len == old_to.len);
        assert(to.buffer == old_to.buffer);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);

        if (old_to.capacity > 0) {
            assert_byte_from_buffer_matches(to.buffer, &old_to_byte);
        }

        if (old_from.len > 0) {
            assert_byte_from_buffer_matches(old_from.ptr, &old_from_byte);
        }
    } else {
        assert(result == AWS_OP_SUCCESS);

        assert(to.len == old_to.len + old_from.len);
        assert(to.buffer == old_to.buffer);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);

        if (old_to.len > 0) {
            assert_byte_from_buffer_matches(to.buffer, &old_to_prefix_byte);
        }

        if (old_from.len > 0) {
            assert_bytes_match(to.buffer + old_to.len, old_from.ptr, old_from.len);
            assert_byte_from_buffer_matches(old_from.ptr, &old_from_byte);
        }
    }

    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    assert(to.buffer == old_to.buffer);
    assert(to.capacity == old_to.capacity);
    assert(to.allocator == old_to.allocator);

    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
