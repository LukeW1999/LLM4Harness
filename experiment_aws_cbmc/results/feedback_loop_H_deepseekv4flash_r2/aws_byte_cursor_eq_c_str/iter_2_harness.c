#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_eq_c_str_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_cursor cursor;
    size_t len_a = nondet_size_t();
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* For c_str, we need a null-terminated string with no embedded nulls */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len < MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Ensure no embedded null bytes before the end */
    for (size_t i = 0; i < c_str_len; i++) {
        __CPROVER_assume(c_str[i] != '\0');
    }
    /* Ensure null termination */
    c_str[c_str_len] = '\0';
    /* Ensure the string is readable */
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));

    /* Save old state */
    struct aws_byte_cursor old_cursor = cursor;

    /* 3. Call function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        /* Contents are equivalent: lengths must match exactly */
        assert(cursor.len == c_str_len);
        if (cursor.len > 0) {
            assert_bytes_match(cursor.ptr, (const uint8_t *)c_str, cursor.len);
        }
    } else {
        /* Contents are not equivalent, but cursor should remain unchanged */
        assert(cursor.len == old_cursor.len);
        assert(cursor.ptr == old_cursor.ptr);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* Clean up */
    free(c_str);
}
