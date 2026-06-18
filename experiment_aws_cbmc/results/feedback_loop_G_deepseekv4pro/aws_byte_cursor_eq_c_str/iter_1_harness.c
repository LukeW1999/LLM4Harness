#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_eq_c_str_harness() {
    /* Initialize a valid aws_byte_cursor */
    struct aws_byte_cursor cursor;
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Construct a null-terminated string c_str */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len < MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(c_str, c_str_len + 1));
    for (size_t i = 0; i <= c_str_len; i++) {
        c_str[i] = nondet_uint8_t();
    }
    c_str[c_str_len] = '\0';
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));

    /* Save cursor state to detect modifications (const correctness) */
    struct aws_byte_cursor old_cursor = cursor;

    /* Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 1. Cursor fields unchanged (const parameter) */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);

    /* 2. Result correctness: matches the logical comparison */
    bool expected = (cursor.len == c_str_len);
    if (expected) {
        for (size_t i = 0; i < cursor.len; i++) {
            if (cursor.ptr[i] != (uint8_t)c_str[i]) {
                expected = false;
                break;
            }
        }
    }
    assert(result == expected);

    /* 3. Validity invariant holds after the call */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* 4. Cleanup */
    free(c_str);
}
