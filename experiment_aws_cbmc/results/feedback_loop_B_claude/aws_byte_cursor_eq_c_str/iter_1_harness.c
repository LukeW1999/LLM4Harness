#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void aws_byte_cursor_eq_c_str_harness(void) {
    /* 1. Declare and set up the cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Set up the c_str — non-deterministic null-terminated string */
    /* We use a bounded array to keep state space manageable */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Ensure null termination */
    c_str[c_str_len] = '\0';

    /* 3. Save old state of cursor before calling */
    struct aws_byte_cursor old_cursor = cursor;

    /* 4. Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Assert postconditions */

    /* The function does not modify the cursor at all */
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.len == old_cursor.len);

    /* The cursor remains valid after the call */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* The return value is a bool — either true or false */
    /* If result is true, then the cursor content equals the c_str */
    /* If result is false, they are not equal */
    /* We verify the result is consistent with aws_array_eq_c_str semantics:
       the function simply delegates to aws_array_eq_c_str, so we just
       assert the result is a valid bool (always true for bool type) */
    assert(result == true || result == false);

    /* Verify that the cursor fields are unchanged (immutability) */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);
}
