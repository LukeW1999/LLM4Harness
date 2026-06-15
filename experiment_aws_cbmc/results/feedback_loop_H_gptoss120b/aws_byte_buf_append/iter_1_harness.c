#include <stddef.h>
#include <stdint.h>
#include "aws/common/byte_buf.h"
#include "aws/common/common.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_append_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&to);
    __CPROVER_assume(aws_byte_buf_is_valid(&to));

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
        /* Success: length increased by the amount copied */
        assert(to.len == old_to.len + from.len);
        /* Unchanged fields of the destination buffer */
        assert(to.buffer == old_to.buffer);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        /* Source cursor must remain unchanged */
        assert(from.len == old_from.len);
        assert(from.ptr == old_from.ptr);
    } else {
        /* Failure: destination buffer must be unchanged */
        assert(to.len == old_to.len);
        assert(to.buffer == old_to.buffer);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        /* Source cursor must remain unchanged */
        assert(from.len == old_from.len);
        assert(from.ptr == old_from.ptr);
    }

    /* 5. Assert validity invariants always hold */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
