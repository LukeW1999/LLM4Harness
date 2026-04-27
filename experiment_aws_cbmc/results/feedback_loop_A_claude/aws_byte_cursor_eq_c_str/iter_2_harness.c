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

    /* 2. Declare a non-deterministic null-terminated C string */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Ensure it's null-terminated at exactly c_str_len */
    c_str[c_str_len] = '\0';
    /* Ensure no null bytes before c_str_len so strlen(c_str) == c_str_len */
    for (size_t i = 0; i < c_str_len; i++) {
        __CPROVER_assume(c_str[i] != '\0');
    }

    /* 3. Save old state */
    struct aws_byte_cursor old_cursor = cursor;

    /* 4. Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Assert postconditions */

    /* The function is a pure comparison — it should not modify the cursor */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);

    /* Validity invariant must still hold */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* The result is a bool — it's either true or false */
    assert(result == true || result == false);

    /* Verify the semantic correctness:
     * If result is true, the cursor length must equal strlen(c_str) == c_str_len */
    if (result) {
        assert(cursor.len == c_str_len);
    }
}
