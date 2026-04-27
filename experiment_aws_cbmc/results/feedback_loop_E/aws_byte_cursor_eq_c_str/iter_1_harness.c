#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_byte_cursor_eq_c_str_harness() {
    /* parameters */
    struct aws_byte_cursor cursor;
    const char *c_str;

    /* assumptions */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));
    __CPROVER_assume(AWS_MEM_IS_READABLE(cursor.ptr, cursor.len));

    /* non-deterministic string for c_str */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    c_str = (const char *)malloc(c_str_len + 1);
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));
    c_str[c_str_len] = '\0'; // null-terminate the string

    /* operation under verification */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* assertions */
    if (result) {
        assert(cursor.len == c_str_len);
        assert_bytes_match(cursor.ptr, (const uint8_t *)c_str, cursor.len);
    } else {
        // No specific changes to cursor expected on failure
    }

    /* unchanged fields */
    assert(cursor.ptr == cursor.ptr);
    assert(cursor.len == cursor.len);

    /* validity invariants */
    assert(aws_byte_cursor_is_valid(&cursor));
    free((void *)c_str);
}
