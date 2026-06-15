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
#include <string.h>
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

    /* Save a copy of the original buffer contents for later comparison */
    uint8_t *old_buffer_copy = NULL;
    if (old_to.capacity > 0) {
        old_buffer_copy = malloc(old_to.capacity);
        __CPROVER_assume(old_buffer_copy != NULL);
        memcpy(old_buffer_copy, old_to.buffer, old_to.capacity);
    }

    /* 3. Call function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success: length increased by the amount copied */
        assert(to.len == old_to.len + from.len);

        /* Buffer contents: the newly copied region matches the source */
        if (from.len > 0) {
            assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
        }

        /* Unchanged prefix of the buffer */
        if (old_to.len > 0) {
            assert_bytes_match(to.buffer, old_buffer_copy, old_to.len);
        }

        /* Unchanged suffix of the buffer */
        size_t suffix_len = 0;
        if (old_to.capacity > old_to.len + from.len) {
            suffix_len = old_to.capacity - (old_to.len + from.len);
            assert_bytes_match(to.buffer + old_to.len + from.len,
                               old_buffer_copy + old_to.len + from.len,
                               suffix_len);
        }
    } else {
        /* Failure: the destination buffer must remain unchanged */
        assert(to.len == old_to.len);
        assert(to.capacity == old_to.capacity);
        assert(to.buffer == old_to.buffer);
        assert(to.allocator == old_to.allocator);
        if (old_to.capacity > 0) {
            assert_bytes_match(to.buffer, old_buffer_copy, old_to.capacity);
        }
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(to.capacity == old_to.capacity);
    assert(to.buffer == old_to.buffer);
    assert(to.allocator == old_to.allocator);
    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
