#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

/* Stub for aws_array_eq_c_str; the function under test delegates to it */
bool aws_array_eq_c_str(const void *const array, const size_t array_len, const char *const c_str) {
    return nondet_bool();
}

void aws_byte_cursor_eq_c_str_harness() {
    /* --- Assumptions about cursor --- */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* --- Assumptions about c_str --- */
    size_t null_index;
    __CPROVER_assume(null_index < MAX_BUFFER_SIZE);
    char *c_str_buffer = (char *)malloc(null_index + 1);
    __CPROVER_assume(c_str_buffer != NULL);
    c_str_buffer[null_index] = '\0'; /* null terminator */
    const char *c_str = c_str_buffer;

    /* --- Save old cursor state --- */
    struct aws_byte_cursor old_cursor = cursor;

    /* --- Call the function --- */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* --- Postconditions --- */
    /* cursor fields must be unchanged */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);
    /* cursor must remain valid */
    assert(aws_byte_cursor_is_valid(&cursor));
    /* The string pointed by c_str is not modified; no assertion needed */

    free(c_str_buffer);
}
