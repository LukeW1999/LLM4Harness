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
    struct aws_byte_cursor from;

    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));

    ensure_byte_buf_has_allocated_buffer_member(&to);
    ensure_byte_cursor_has_allocated_buffer_member(&from);

    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* 2. Save old state BEFORE calling */
    struct aws_byte_buf old_to = to;
    struct store_byte_from_buffer old_to_buf;
    if (to.buffer != NULL && to.capacity > 0) {
        save_byte_from_array(to.buffer, to.capacity, &old_to_buf);
    }

    struct aws_byte_cursor old_from = from; /* cursor is immutable, just for completeness */

    /* 3. Call function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Changed field: length increased by the amount copied */
        assert(to.len == old_to.len + from.len);

        /* Buffer contents: the newly copied region matches source */
        if (from.len > 0) {
            assert(to.buffer != NULL);
            assert(from.ptr != NULL);
            assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
        }

        /* Unchanged bytes before the copied region */
        if (old_to.len > 0) {
            assert_byte_from_buffer_matches(to.buffer, &old_to_buf);
        }

        /* Unchanged bytes after the new length (if any) */
        if (to.capacity > to.len) {
            size_t unchanged_len = to.capacity - to.len;
            if (unchanged_len > 0) {
                /* compare the tail region with the saved old bytes */
                assert_bytes_match(to.buffer + to.len,
                                   old_to.buffer + to.len,
                                   unchanged_len);
            }
        }
    } else {
        /* Failure: the destination buffer must be unchanged */
        assert(to.len == old_to.len);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        assert(to.buffer == old_to.buffer);

        /* Buffer contents unchanged */
        if (to.buffer != NULL && to.capacity > 0) {
            assert_byte_from_buffer_matches(to.buffer, &old_to_buf);
        }

        /* Source cursor unchanged */
        assert(from.len == old_from.len);
        assert(from.ptr == old_from.ptr);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(to.capacity == old_to.capacity);
    assert(to.allocator == old_to.allocator);
    assert(to.buffer == old_to.buffer);
    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    /* 6. Assert validity invariants always hold */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
