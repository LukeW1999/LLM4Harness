#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_advance_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_cursor *cursor = malloc(sizeof(*cursor));
    size_t len;

    /* Preconditions: cursor not NULL and cursor is valid */
    __CPROVER_assume(cursor != NULL);
    __CPROVER_assume(aws_byte_cursor_is_valid(cursor));

    /* Save old state */
    struct aws_byte_cursor old_cursor = *cursor;

    /* Call the function */
    struct aws_byte_cursor ret = aws_byte_cursor_advance(cursor, len);

    /* Assert postconditions */
    if (len <= old_cursor.len) {
        /* Success: returned cursor points to the advanced portion */
        assert(ret.ptr == old_cursor.ptr);
        assert(ret.len == len);
        /* The original cursor is advanced */
        assert(cursor->ptr == old_cursor.ptr + len);
        assert(cursor->len == old_cursor.len - len);
    } else {
        /* Failure: returned cursor is empty, original unchanged */
        assert(ret.ptr == NULL);
        assert(ret.len == 0);
        assert(cursor->ptr == old_cursor.ptr);
        assert(cursor->len == old_cursor.len);
    }

    /* Validity invariant */
    assert(aws_byte_cursor_is_valid(cursor));

    /* Cleanup */
    free(cursor);
}
