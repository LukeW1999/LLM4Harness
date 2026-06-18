#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_advance_harness(void) {
    /* 1. Set up cursor */
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
    if (old_cursor.len >= len) {
        /* Advance succeeded: result is valid slice of length len */
        assert(aws_byte_cursor_is_valid(&result));
        assert(result.len == len);
        if (len > 0) {
            assert(result.ptr != NULL);
            assert(result.ptr == old_cursor.ptr);
        }
        /* cursor advanced by len */
        assert(cursor.len == old_cursor.len - len);
        if (old_cursor.ptr != NULL) {
            assert(cursor.ptr == old_cursor.ptr + len);
        }
    } else {
        /* Advance failed: result is empty cursor */
        assert(result.ptr == NULL);
        assert(result.len == 0);
        /* cursor unchanged */
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    /* 6. cursor validity invariant still holds */
    assert(aws_byte_cursor_is_valid(&cursor));
}
