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

void aws_byte_buf_append_harness() {
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

    /* 3. Call function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Changed fields */
        assert(to.len == old_to.len + from.len);

        /* Unchanged fields of the destination buffer */
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        assert(to.buffer == old_to.buffer);

        /* Unchanged fields of the source cursor */
        assert(from.ptr == old_from.ptr);
        assert(from.len == old_from.len);

        /* Verify that the existing bytes before the append are unchanged */
        if (old_to.len > 0) {
            assert_bytes_match(to.buffer, old_to.buffer, old_to.len);
        }

        /* Verify that the appended region matches the source cursor */
        if (from.len > 0) {
            assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
        }

        /* Verify that the bytes after the new length are unchanged */
        size_t tail_len = to.capacity - (old_to.len + from.len);
        if (tail_len > 0) {
            assert_bytes_match(to.buffer + old_to.len + from.len,
                               old_to.buffer + old_to.len + from.len,
                               tail_len);
        }
    } else {
        /* On failure the destination buffer must be unchanged */
        assert(to.len == old_to.len);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        assert(to.buffer == old_to.buffer);
        if (to.capacity > 0) {
            assert_bytes_match(to.buffer, old_to.buffer, to.capacity);
        }

        /* The source cursor must be unchanged */
        assert(from.ptr == old_from.ptr);
        assert(from.len == old_from.len);
    }

    /* 5. Assert fields that must NOT change regardless of result (already covered) */
    assert(to.allocator == old_to.allocator);
    assert(to.capacity == old_to.capacity);
    assert(to.buffer == old_to.buffer);
    assert(from.ptr == old_from.ptr);
    assert(from.len == old_from.len);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
