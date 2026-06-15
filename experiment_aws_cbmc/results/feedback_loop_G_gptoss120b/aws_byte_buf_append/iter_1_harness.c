#include <assert.h>
#include <stddef.h>
#include "aws/common/byte_buf.h"
#include "aws/common/common.h"
#include "proof_helpers/make_common_data_structures.h"

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
    struct aws_byte_cursor old_from = from;

    /* 3. Call function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Changed fields */
        assert(to.len == old_to.len + from.len);
        if (from.len > 0) {
            assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
        }

        /* Unchanged fields of 'to' */
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        assert(to.buffer == old_to.buffer);

        /* Unchanged fields of 'from' */
        assert(from.len == old_from.len);
        assert(from.ptr == old_from.ptr);
    } else {
        /* On failure, the structures must be unchanged */
        assert(to.len == old_to.len);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        assert(to.buffer == old_to.buffer);

        assert(from.len == old_from.len);
        assert(from.ptr == old_from.ptr);
    }

    /* 5. Assert fields that must NOT change regardless of result (redundant with above) */
    assert(to.allocator == old_to.allocator);
    assert(to.capacity == old_to.capacity);
    assert(to.buffer == old_to.buffer);
    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
