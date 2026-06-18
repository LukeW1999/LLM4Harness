#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>

void aws_byte_buf_append_harness() {
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    /* Bound and allocate valid structures */
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&to);
    ensure_byte_cursor_has_allocated_buffer_member(&from);
    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* Save old state */
    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    size_t old_to_len = to.len;

    /* Save a byte from the prefix of to (the part that must remain unchanged) */
    struct store_byte_from_buffer old_byte_from_to;
    if (to.buffer != NULL && old_to_len > 0) {
        save_byte_from_array(to.buffer, old_to_len, &old_byte_from_to);
    }

    /* Save a byte from from's data (which must never change) */
    struct store_byte_from_buffer old_byte_from_from;
    if (from.len > 0) {
        save_byte_from_array(from.ptr, from.len, &old_byte_from_from);
    }

    /* Call function under verification */
    int rv = aws_byte_buf_append(&to, &from);

    /* Postconditions based on return value */
    if (rv == AWS_OP_SUCCESS) {
        /* to.len must increase by from.len */
        assert(to.len == old_to_len + from.len);

        /* The appended bytes must match from's data */
        if (from.len > 0) {
            assert_bytes_match(to.buffer + old_to_len, from.ptr, from.len);
        }

        /* Part of to before old_to_len must be unchanged */
        if (to.buffer != NULL && old_to_len > 0) {
            assert_byte_from_buffer_matches(to.buffer, &old_byte_from_to);
        }
    } else {
        /* On failure, to is completely unchanged */
        assert(to.len == old_to_len);
        if (to.buffer != NULL && old_to_len > 0) {
            assert_byte_from_buffer_matches(to.buffer, &old_byte_from_to);
        }
    }

    /* from must never be modified */
    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);
    if (from.len > 0) {
        assert_byte_from_buffer_matches(from.ptr, &old_byte_from_from);
    }

    /* Fields of to that must never change */
    assert(to.capacity == old_to.capacity);
    assert(to.allocator == old_to.allocator);
    assert(to.buffer == old_to.buffer);

    /* Validity invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
