#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_append_harness() {
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    /* Ensure the byte_buf is bounded and has allocated buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&to);
    __CPROVER_assume(aws_byte_buf_is_valid(&to));

    /* Ensure the byte_cursor is bounded and has allocated buffer */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&from);
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* Use the default allocator (not can_fail_allocator) */
    to.allocator = aws_default_allocator();

    /* Save old state for immutability checks */
    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;
    struct store_byte_from_buffer old_to_buffer;
    bool has_old_to_buffer = false;
    if (old_to.len > 0) {
        save_byte_from_array(to.buffer, old_to.len, &old_to_buffer);
        has_old_to_buffer = true;
    }

    /* Call the function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* Assert validity invariants always hold */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));

    /* Assert unchanged fields for 'to' */
    assert(to.allocator == old_to.allocator);
    assert(to.capacity == old_to.capacity);
    assert(to.buffer == old_to.buffer);

    /* Assert unchanged fields for 'from' */
    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    if (result == AWS_OP_SUCCESS) {
        /* On success, to.len increases by from.len */
        assert(to.len == old_to.len + from.len);
        /* The copied data matches the source */
        assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
    } else {
        /* On failure, to.len remains unchanged */
        assert(to.len == old_to.len);
        /* The buffer content before old_to.len remains unchanged (if any) */
        if (has_old_to_buffer) {
            assert_byte_from_buffer_matches(to.buffer, &old_to_buffer);
        }
    }
}
