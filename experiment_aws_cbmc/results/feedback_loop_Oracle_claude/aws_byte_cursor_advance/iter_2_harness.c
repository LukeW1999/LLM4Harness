#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_advance_harness(void) {
    /* Allocate and initialize a cursor with unconstrained values */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Save original values for frame condition checks */
    size_t original_len = cursor.len;
    uint8_t *original_ptr = cursor.ptr;

    /* Unconstrained advance length */
    size_t len;

    /* Call the function under test */
    struct aws_byte_cursor rv = aws_byte_cursor_advance(&cursor, len);

    /* Postcondition 1: The returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&rv));

    /* Postcondition 2: The input cursor must still be valid after the call */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* Postcondition 3: Check correctness based on overflow/bounds conditions */
    if (original_len > (SIZE_MAX >> 1) || len > (SIZE_MAX >> 1) || len > original_len) {
        /* Failure case: returned cursor should be empty (ptr=NULL, len=0) */
        assert(rv.ptr == NULL);
        assert(rv.len == 0);

        /* Frame condition: cursor should be unchanged on failure */
        assert(cursor.ptr == original_ptr);
        assert(cursor.len == original_len);
    } else {
        /* Success case: returned cursor points to original start with length len */
        assert(rv.ptr == original_ptr);
        assert(rv.len == len);

        /* Frame condition: cursor was advanced by len bytes */
        if (original_ptr == NULL) {
            assert(cursor.ptr == NULL);
        } else {
            assert(cursor.ptr == original_ptr + len);
        }
        assert(cursor.len == original_len - len);

        /* Length invariant: cursor length decreased by exactly len */
        assert(cursor.len + len == original_len);
    }
}
