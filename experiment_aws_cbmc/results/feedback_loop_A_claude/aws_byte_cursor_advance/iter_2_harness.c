#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_advance_harness(void) {
    /* Set up a cursor */
    struct aws_byte_cursor cursor;
    size_t len;

    /* Make cursor valid with bounded size */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Nondet length to advance */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Save old state */
    struct aws_byte_cursor old_cursor = cursor;

    /* Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_advance(&cursor, len);

    /* Postconditions */
    if (len <= old_cursor.len) {
        /* Advance succeeded */
        assert(result.ptr == old_cursor.ptr);
        assert(result.len == len);
        assert(cursor.ptr == old_cursor.ptr + len);
        assert(cursor.len == old_cursor.len - len);
        assert(aws_byte_cursor_is_valid(&result));
        assert(aws_byte_cursor_is_valid(&cursor));
    } else {
        /* Advance failed - result is zeroed */
        assert(result.ptr == NULL);
        assert(result.len == 0);
        /* cursor unchanged */
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }
}
