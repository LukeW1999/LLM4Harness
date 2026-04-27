#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_advance_harness() {
    struct aws_byte_cursor cursor;
    size_t len;
    struct aws_byte_cursor old_cursor;
    size_t old_len;

    // Initialize cursor with allocated buffer
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    // Store old values of cursor and len
    old_cursor = cursor;
    old_len = len;

    // Ensure len is within a reasonable range
    __CPROVER_assume(len <= old_cursor.len);

    int result = aws_byte_cursor_advance(&cursor, len);

    // Check frame conditions and postconditions
    if (result == AWS_OP_SUCCESS) {
        assert(cursor.ptr == old_cursor.ptr + old_len);
        assert(cursor.len == old_cursor.len - old_len);
    } else {
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    // Validity invariants
    assert(aws_byte_cursor_is_valid(&cursor));
}
