#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_advance_harness() {
    /* parameters */
    struct aws_byte_cursor cursor;
    size_t len;

    /* assumptions */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* save old state */
    struct aws_byte_cursor old_cursor = cursor;

    /* operation under verification */
    struct aws_byte_cursor result = aws_byte_cursor_advance(&cursor, len);

    /* assertions */
    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_cursor_is_valid(&result));

    if (len <= old_cursor.len && len <= (SIZE_MAX >> 1) && old_cursor.len <= (SIZE_MAX >> 1)) {
        /* success case */
        assert(result.ptr == old_cursor.ptr);
        assert(result.len == len);
        assert(cursor.ptr == old_cursor.ptr + len);
        assert(cursor.len == old_cursor.len - len);
    } else {
        /* failure case */
        assert(result.ptr == NULL);
        assert(result.len == 0);
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    /* unchanged fields */
    assert(cursor.allocator == old_cursor.allocator);
}
