#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 16

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* Setup */
    struct aws_byte_cursor cursor;
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Make a nondet cursor with bounded size */
    cursor.len = nondet_size_t();
    cursor.ptr = NULL;
    if (cursor.len > 0) {
        cursor.ptr = malloc(cursor.len);
        __CPROVER_assume(cursor.ptr != NULL);
    }

    /* Ground-truth preconditions */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Save original cursor state for frame condition checks */
    size_t old_cursor_len = cursor.len;
    uint8_t *old_cursor_ptr = cursor.ptr;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, cursor);

    /* Postcondition 1: Return value correctness */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* Postcondition 2: On success, output buffer invariants */
    if (result == AWS_OP_SUCCESS) {
        /* Length and capacity must equal the source cursor length */
        assert(dest.len == cursor.len);
        assert(dest.capacity == cursor.len);

        /* Allocator must be set correctly */
        assert(dest.allocator == allocator);

        /* The resulting buffer must be valid */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* Postcondition 3: Frame condition - cursor is not modified by the function */
    assert(cursor.len == old_cursor_len);
    assert(cursor.ptr == old_cursor_ptr);

    /* Cleanup */
    if (result == AWS_OP_SUCCESS) {
        aws_byte_buf_clean_up(&dest);
    }
}
