#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_advance_harness(void) {
    /* Allocate and initialize a non-deterministic aws_byte_cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Non-deterministic advance length */
    size_t len;

    /* Save original state for postcondition checks */
    size_t old_len = cursor.len;
    uint8_t *old_ptr = cursor.ptr;

    /* Call the function under test */
    struct aws_byte_cursor rv = aws_byte_cursor_advance(&cursor, len);

    /* Postcondition: cursor is still valid */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* Postcondition: return value is valid */
    assert(aws_byte_cursor_is_valid(&rv));

    /* Determine which branch was taken */
    bool overflow_condition = (old_len > (SIZE_MAX >> 1)) ||
                              (len > (SIZE_MAX >> 1)) ||
                              (len > old_len);

    if (overflow_condition) {
        /* Failure path: rv must be empty cursor */
        assert(rv.ptr == NULL);
        assert(rv.len == 0);

        /* Failure path: cursor must be unchanged */
        assert(cursor.ptr == old_ptr);
        assert(cursor.len == old_len);
    } else {
        /* Success path: rv points to original start */
        assert(rv.ptr == old_ptr);
        assert(rv.len == len);

        /* Success path: cursor advanced by len */
        if (old_ptr == NULL) {
            assert(cursor.ptr == NULL);
        } else {
            assert(cursor.ptr == old_ptr + len);
        }
        assert(cursor.len == old_len - len);

        /* Length invariant: consumed + remaining == original */
        assert(rv.len + cursor.len == old_len);
    }
}

void aws_byte_cursor_advance_harness(void) {
    aws_byte_cursor_advance_harness();
    return 0;
}
