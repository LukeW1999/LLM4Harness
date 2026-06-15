#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
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

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    /* 3. Call function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(to.len == old_to.len + from.len);
        assert(to.buffer == old_to.buffer);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        assert(from.len == old_from.len);
        assert(from.ptr == old_from.ptr);
    } else {
        assert(to.len == old_to.len);
        assert(to.buffer == old_to.buffer);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        assert(from.len == old_from.len);
        assert(from.ptr == old_from.ptr);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(to.allocator == old_to.allocator);
    assert(to.capacity == old_to.capacity);
    assert(from.ptr == old_from.ptr);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
