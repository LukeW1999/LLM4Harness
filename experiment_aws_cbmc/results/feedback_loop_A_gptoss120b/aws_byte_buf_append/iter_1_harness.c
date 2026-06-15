#include <assert.h>
#include "aws/common/byte_buf.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_append_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf to;
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&to);
    __CPROVER_assume(aws_byte_buf_is_valid(&to));

    struct aws_byte_cursor from;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&from);
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* 2. Save old state */
    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    /* 3. Call function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* 4. Assert postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* changed field */
        assert(to.len == old_to.len + from.len);

        /* content of the appended region must match source */
        if (from.len > 0) {
            assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
        }

        /* unchanged fields of 'to' */
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        assert(to.buffer == old_to.buffer);

        /* unchanged fields of 'from' */
        assert(from.len == old_from.len);
        assert(from.ptr == old_from.ptr);
    } else {
        /* on failure the structures must be unchanged */
        assert(to.len == old_to.len);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        assert(to.buffer == old_to.buffer);

        assert(from.len == old_from.len);
        assert(from.ptr == old_from.ptr);
    }

    /* 5. Validity invariants must always hold */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
