#include "proof_helpers/make_common_data_structures.h"
#include "aws/common/byte_buf.h"
#include <assert.h>

void aws_byte_buf_append_harness() {
    /* nondet inputs */
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    /* bound sizes */
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));

    /* allocate buffer members */
    ensure_byte_buf_has_allocated_buffer_member(&to);
    ensure_byte_cursor_has_allocated_buffer_member(&from);

    /* assume validity */
    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* ensure memory regions are accessible */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(to.buffer, to.capacity));
    __CPROVER_assume(AWS_MEM_IS_READABLE(from.ptr, from.len));

    /* avoid overlapping regions (undefined behavior for memcpy) */
    __CPROVER_assume(to.buffer == NULL || from.ptr == NULL || 
                     from.ptr + from.len <= to.buffer + to.len ||
                     to.buffer + to.len + from.len <= from.ptr);

    /* save old state */
    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    /* call function */
    int result = aws_byte_buf_append(&to, &from);

    /* validity invariants */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));

    /* unchanged fields */
    assert(to.buffer == old_to.buffer);
    assert(to.capacity == old_to.capacity);
    assert(to.allocator == old_to.allocator);
    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    if (result == AWS_OP_SUCCESS) {
        /* success: len increased by from.len */
        assert(to.len == old_to.len + from.len);

        /* copied bytes match */
        if (from.len > 0) {
            assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
        }

        /* prefix unchanged */
        if (old_to.len > 0) {
            assert_bytes_match(to.buffer, old_to.buffer, old_to.len);
        }

        /* suffix unchanged */
        if (old_to.len + from.len < to.capacity) {
            assert_bytes_match(to.buffer + old_to.len + from.len,
                              old_to.buffer + old_to.len + from.len,
                              to.capacity - (old_to.len + from.len));
        }
    } else {
        /* failure: dest too small, nothing copied */
        assert(to.len == old_to.len);

        /* buffer unchanged */
        if (to.capacity > 0) {
            assert_bytes_match(to.buffer, old_to.buffer, to.capacity);
        }
    }
}
