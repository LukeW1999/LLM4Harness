#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_advance_harness(void) {
    /* Allocate and initialize a non-deterministic aws_byte_cursor */
    struct aws_byte_cursor cursor;
    
    /* Ensure cursor is valid: either ptr is NULL and len is 0, or ptr is non-NULL */
    size_t cursor_len;
    uint8_t *cursor_ptr;
    __CPROVER_assume(cursor_len <= (SIZE_MAX >> 1));
    
    /* If len > 0, ptr must be non-NULL */
    if (cursor_len == 0) {
        cursor.ptr = NULL;
        cursor.len = 0;
    } else {
        cursor.ptr = malloc(cursor_len);
        __CPROVER_assume(cursor.ptr != NULL);
        cursor.len = cursor_len;
    }
    
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Non-deterministic advance length */
    size_t len;
    __CPROVER_assume(len <= (SIZE_MAX >> 1));

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
    bool overflow_condition = (len > old_len);

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
