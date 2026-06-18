#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_append_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf to;
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&to);
    __CPROVER_assume(aws_byte_buf_is_valid(&to));

    struct aws_byte_cursor from;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&from);
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* 2. Save old state BEFORE calling */
    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    struct store_byte_from_buffer old_to_buf_storage;
    if (to.capacity > 0) {
        save_byte_from_array(to.buffer, to.capacity, &old_to_buf_storage);
    }

    struct store_byte_from_buffer old_from_buf_storage;
    if (from.len > 0) {
        save_byte_from_array(from.ptr, from.len, &old_from_buf_storage);
    }

    /* 3. Call function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success: length increased by from.len */
        assert(to.len == old_to.len + from.len);

        /* Buffer content before the copy region unchanged */
        if (old_to.len > 0) {
            assert_bytes_match(to.buffer, old_to.buffer, old_to.len);
        }

        /* Copied bytes match source cursor */
        if (from.len > 0) {
            assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
        }

        /* Buffer content after the copy region unchanged */
        if (to.capacity > old_to.len + from.len) {
            size_t after_len = to.capacity - (old_to.len + from.len);
            assert_bytes_match(to.buffer + old_to.len + from.len,
                               old_to.buffer + old_to.len + from.len,
                               after_len);
        }
    } else {
        /* Failure: destination buffer unchanged */
        assert(to.len == old_to.len);
        assert(to.buffer == old_to.buffer);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        if (to.capacity > 0) {
            assert_bytes_match(to.buffer, old_to.buffer, to.capacity);
        }

        /* Source cursor unchanged */
        assert(from.len == old_from.len);
        assert(from.ptr == old_from.ptr);
        if (from.len > 0) {
            assert_bytes_match(from.ptr, old_from.ptr, from.len);
        }
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(to.capacity == old_to.capacity);
    assert(to.allocator == old_to.allocator);
    assert(to.buffer == old_to.buffer);
    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
