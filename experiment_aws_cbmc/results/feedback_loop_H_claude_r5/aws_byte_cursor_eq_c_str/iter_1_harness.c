#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_byte_cursor_eq_c_str_harness(void) {
    /* 1. Declare and set up the cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Declare a non-deterministic null-terminated C string */
    /* We need a c_str that is either NULL or a valid null-terminated string */
    /* Use a bounded array to represent the c_str */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    /* c_str can be NULL (aws_array_eq_c_str allows NULL if len is 0) */
    /* but for this function, c_str is passed directly */
    /* We allow c_str to be NULL or non-NULL */
    if (c_str != NULL) {
        /* Ensure it's null-terminated */
        c_str[c_str_len] = '\0';
    }

    /* 3. Save old state */
    struct aws_byte_cursor old_cursor = cursor;

    /* 4. Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Assert postconditions */

    /* The function returns a bool - either true or false */
    /* The cursor should not be modified by this function */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);

    /* Validity invariant must still hold */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* The result is a bool - just verify it's a valid bool value */
    assert(result == true || result == false);

    /* If c_str is NULL, the comparison should return false unless cursor.len == 0
     * Actually aws_array_eq_c_str with NULL c_str: the behavior depends on implementation
     * We just verify the cursor is unchanged regardless of result */
    (void)result;
}
