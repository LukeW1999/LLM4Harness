#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_advance_harness(void) {
    /* 1. Set up a cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Nondeterministic length to advance */
    size_t len;

    /* 3. Save cursor state before call */
    struct aws_byte_cursor cursor_old = cursor;

    /* 4. Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_advance(&cursor, len);

    /* 5. Assert postconditions */
    if (len <= cursor_old.len && cursor_old.ptr != NULL) {
        /* Success: returned cursor has the first len bytes */
        assert(result.len == len);
        assert(result.ptr == cursor_old.ptr);
        /* cursor advanced by len */
        assert(cursor.len == cursor_old.len - len);
        if (len > 0) {
            assert(cursor.ptr == cursor_old.ptr + len);
        }
        assert(aws_byte_cursor_is_valid(&result));
        assert(aws_byte_cursor_is_valid(&cursor));
    } else {
        /* Failure: returned cursor is zeroed */
        assert(result.ptr == NULL);
        assert(result.len == 0);
        /* original cursor unchanged */
        assert(cursor.ptr == cursor_old.ptr);
        assert(cursor.len == cursor_old.len);
    }
}
