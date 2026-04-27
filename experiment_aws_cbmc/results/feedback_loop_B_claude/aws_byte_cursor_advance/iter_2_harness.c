#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_advance_harness(void) {
    /* 1. Declare and set up the cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Nondet length to advance */
    size_t len;

    /* 3. Save old cursor state */
    struct aws_byte_cursor old_cursor = cursor;

    /* 4. Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_advance(&cursor, len);

    /* 5. Assert postconditions */
    if (len <= old_cursor.len) {
        /* Advance succeeded: result is valid slice of old cursor */
        assert(aws_byte_cursor_is_valid(&result));
        assert(result.len == len);
        if (len > 0) {
            assert(result.ptr == old_cursor.ptr);
        }
        /* cursor is advanced */
        assert(aws_byte_cursor_is_valid(&cursor));
        assert(cursor.len == old_cursor.len - len);
        if (old_cursor.len - len > 0) {
            assert(cursor.ptr == old_cursor.ptr + len);
        }
    } else {
        /* Advance failed: result is zeroed */
        assert(result.ptr == NULL);
        assert(result.len == 0);
        /* cursor is unchanged */
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    /* 6. cursor validity invariant must still hold */
    assert(aws_byte_cursor_is_valid(&cursor));
}
