#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"
#include "aws/common/byte_buf.h"

void aws_byte_buf_append_harness() {
    /* Declare and bound data structures */
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    /* Bound the structures to limit state space */
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));

    /* Allocate buffer members non-deterministically */
    ensure_byte_buf_has_allocated_buffer_member(&to);
    ensure_byte_cursor_has_allocated_buffer_member(&from);

    /* Assume validity preconditions */
    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* Save old state for immutability checks */
    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;
    struct store_byte_from_buffer old_to_buffer;
    save_byte_from_array(to.buffer, to.len, &old_to_buffer);

    /* Call function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* Assert validity invariants always hold */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));

    /* Assert unchanged fields (implicit invariants) */
    assert(to.allocator == old_to.allocator);
    assert(to.capacity == old_to.capacity);
    assert(to.buffer == old_to.buffer);
    assert(from.ptr == old_from.ptr);
    assert(from.len == old_from.len);

    if (result == AWS_OP_SUCCESS) {
        /* On success, to.len increases by from.len */
        assert(to.len == old_to.len + from.len);
        /* The copied data matches the source */
        assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
    } else {
        /* On failure, to.len is unchanged */
        assert(to.len == old_to.len);
        /* The buffer content is unchanged */
        assert_byte_from_buffer_matches(to.buffer, &old_to_buffer);
    }
}
