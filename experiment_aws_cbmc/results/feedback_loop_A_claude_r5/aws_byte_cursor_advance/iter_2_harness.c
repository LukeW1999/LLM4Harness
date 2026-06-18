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

    /* 3. Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_advance(&cursor, len);

    /* 4. Assert postconditions */
    if (len <= cursor.len) {
        /* Should return a valid sub-cursor */
        assert(aws_byte_cursor_is_valid(&result));
        assert(result.len == len);
    } else {
        /* Should return empty cursor */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    }

    /* 5. cursor validity invariant still holds */
    assert(aws_byte_cursor_is_valid(&cursor));
}
