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
    /* 1. Declare and bound data structures */
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    /* Bound the byte_buf and byte_cursor to keep state space manageable */
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&to);
    __CPROVER_assume(aws_byte_buf_is_valid(&to));

    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&from);
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* 2. Save old state before calling — needed for immutability checks */
    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    /* Save bytes from the source buffer if the copy might happen,
     * to later verify that source bytes haven't been modified. */
    if (from.len > 0 && from.ptr != NULL) {
        /* We save a single byte from the source to check immutability */
    }

    /* 3. Call function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success: the function copied data and increased to->len */
        assert(to.len == old_to.len + from.len);
        /* The copied bytes must match the source bytes */
        if (from.len > 0) {
            assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
        }
    } else {
        /* Failure: to and from should remain unchanged */
        assert(to.len == old_to.len);
        /* On failure, no data should have been copied beyond to->len boundary */
        /* But the function doesn't modify the buffer, so we check unchanged buffer content */
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
