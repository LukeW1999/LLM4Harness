#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include "aws_byte_buf.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_append_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    /* 2. Bound the structure (limits CBMC state space) */
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));

    /* 3. Allocate pointer members */
    ensure_byte_buf_has_allocated_buffer_member(&to);
    ensure_byte_cursor_has_allocated_buffer_member(&from);

    /* 4. Assume validity precondition */
    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* 5. Save old state (for checking immutability) */
    struct aws_byte_buf old_to = to;
    struct store_byte_from_buffer old_byte_to;
    save_byte_from_array(to.buffer, to.len, &old_byte_to);

    struct aws_byte_cursor old_from = from;
    struct store_byte_from_buffer old_byte_from;
    save_byte_from_array(from.ptr, from.len, &old_byte_from);

    /* 6. Assume function-specific preconditions */
    // No additional specific preconditions needed beyond validity

    /* 7. Call function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* 8. Assert postconditions (both branches) */
    if (result == AWS_OP_SUCCESS) {
        assert(to.len == old_to.len + from.len);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        assert_bytes_match(to.buffer, old_to.buffer, old_to.len);
        assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
    } else {
        assert(result == AWS_OP_ERR);
        assert(to.len == old_to.len);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        assert_bytes_match(to.buffer, old_to.buffer, old_to.len);
    }

    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
    assert_byte_from_buffer_matches(to.buffer, &old_byte_to);
    assert_byte_from_buffer_matches(from.ptr, &old_byte_from);
}
