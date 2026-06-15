#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_advance_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_cursor cursor;
    size_t len;

    /* Validate cursor */
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Save old cursor */
    struct aws_byte_cursor old_cursor = cursor;

    /* Call function */
    struct aws_byte_cursor result = aws_byte_cursor_advance(&cursor, len);

    /* Postconditions */
    if (old_cursor.len >= len) {
        /* Success: cursor advanced by len */
        assert(cursor.ptr == old_cursor.ptr + len);
        assert(cursor.len == old_cursor.len - len);
        assert(result.ptr == old_cursor.ptr);
        assert(result.len == len);
    } else {
        /* Failure: cursor and result are zeroed */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
        assert(result.ptr == NULL);
        assert(result.len == 0);
    }

    /* Validity of cursor and result */
    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_cursor_is_valid(&result));
    /* Old cursor unchanged */
    assert(aws_byte_cursor_is_valid(&old_cursor));
}
