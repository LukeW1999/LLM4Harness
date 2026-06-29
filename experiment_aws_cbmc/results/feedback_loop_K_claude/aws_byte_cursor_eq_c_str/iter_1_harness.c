#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_CURSOR_LEN 8
#define MAX_CSTR_LEN   8

void aws_byte_cursor_eq_c_str_harness(void) {
    /* Allocate and initialize the aws_byte_cursor */
    struct aws_byte_cursor cursor;
    size_t cursor_len;
    __CPROVER_assume(cursor_len <= MAX_CURSOR_LEN);
    cursor.len = cursor_len;

    /* Allocate backing memory for cursor */
    uint8_t *cursor_ptr = malloc(cursor_len == 0 ? 1 : cursor_len);
    __CPROVER_assume(cursor_ptr != NULL);
    cursor.ptr = (cursor_len == 0) ? NULL : cursor_ptr;

    /* Precondition: cursor must be valid */
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Allocate and initialize the c_str */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_CSTR_LEN);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Ensure null terminator */
    c_str[c_str_len] = '\0';

    /* Save original values for frame condition checks */
    size_t original_cursor_len = cursor.len;
    uint8_t *original_cursor_ptr = cursor.ptr;

    /* Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* Postcondition: frame conditions - cursor is not modified */
    assert(cursor.len == original_cursor_len);
    assert(cursor.ptr == original_cursor_ptr);

    /* Postcondition: result type is bool */
    assert(result == true || result == false);

    /* Postcondition: verify correctness of result
     * The function should return true iff cursor->len == strlen(c_str)
     * AND the bytes match.
     * We verify the relationship between result and aws_array_eq_c_str. */
    bool expected = aws_array_eq_c_str(cursor.ptr, cursor.len, c_str);
    assert(result == expected);
}
