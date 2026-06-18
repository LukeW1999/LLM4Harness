#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_advance_harness(void) {
    /* Set up cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Nondeterministic length to advance */
    size_t len;

    /* Save old cursor state */
    struct aws_byte_cursor old_cursor = cursor;

    /* Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_advance(&cursor, len);

    /* Postconditions */
    if (len <= old_cursor.len) {
        /* Advance succeeded: result should be valid slice */
        assert(aws_byte_cursor_is_valid(&result));
        assert(result.len == len);
        if (len > 0) {
            assert(result.ptr == old_cursor.ptr);
        }
        /* cursor should be advanced */
        assert(cursor.len == old_cursor.len - len);
        assert(aws_byte_cursor_is_valid(&cursor));
    } else {
        /* Advance failed: result should be empty/null cursor */
        assert(result.ptr == NULL);
        assert(result.len == 0);
        /* cursor should be unchanged */
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }
}
