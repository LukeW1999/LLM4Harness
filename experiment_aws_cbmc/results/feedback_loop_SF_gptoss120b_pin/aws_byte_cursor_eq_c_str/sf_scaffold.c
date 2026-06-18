#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_eq_c_str_harness(void) {
    struct aws_byte_cursor cursor;
    /* nondeterministic length for the cursor */
    cursor.len = (size_t)nondet_uint();
    __CPROVER_assume(cursor.len <= 1024U);

    /* allocate memory for cursor.ptr */
    cursor.ptr = malloc(cursor.len);
    __CPROVER_assume(cursor.ptr != NULL || cursor.len == 0U);

    /* nondeterministic length for the C string (excluding null terminator) */
    size_t c_str_len = (size_t)nondet_uint();
    __CPROVER_assume(c_str_len <= 1024U);

    /* allocate memory for the C string (including null terminator) */
    char *c_str = malloc(c_str_len + 1U);
    __CPROVER_assume(c_str != NULL);

    /* ensure null termination */
    __CPROVER_assume(c_str[c_str_len] == '\0');

    /* precondition: cursor must be valid */
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* snapshot of input state */
    size_t old_len = cursor.len;
    uint8_t *old_ptr = cursor.ptr;
    char *old_c_str = c_str;

    /* call the function under verification */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* ASSERT_POSTCONDITIONS_HERE */
}
