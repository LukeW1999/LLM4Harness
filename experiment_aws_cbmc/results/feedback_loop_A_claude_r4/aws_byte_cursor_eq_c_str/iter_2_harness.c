#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_byte_cursor_eq_c_str_harness(void) {
    /* 1. Set up the aws_byte_cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Set up the c_str (null-terminated string) - must be non-NULL per precondition */
    char *c_str = (char *)malloc(MAX_BUFFER_SIZE + 1);
    __CPROVER_assume(c_str != NULL);
    /* Ensure it's null-terminated somewhere within bounds */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    /* Force a null terminator at position c_str_len */
    c_str[c_str_len] = '\0';

    /* 3. Save old state of cursor before the call */
    struct aws_byte_cursor old_cursor = cursor;

    /* 4. Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, (const char *)c_str);

    /* 5. Assert postconditions */

    /* The cursor should NOT be modified by this function */
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.len == old_cursor.len);

    /* Validity invariant must still hold */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* The result is a valid bool */
    assert(result == true || result == false);
}
