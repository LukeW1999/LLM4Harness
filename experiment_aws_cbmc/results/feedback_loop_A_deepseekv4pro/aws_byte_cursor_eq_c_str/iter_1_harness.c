#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_eq_c_str_harness() {
    /* 1. Declare and bound the cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Declare and bound the null-terminated c_str */
    size_t c_str_buf_len;
    __CPROVER_assume(c_str_buf_len <= MAX_BUFFER_SIZE);
    char *c_str_buf = malloc(c_str_buf_len + 1);
    __CPROVER_assume(c_str_buf != NULL);
    for (size_t i = 0; i < c_str_buf_len; i++) {
        c_str_buf[i] = nondet_uint8_t();
    }
    c_str_buf[c_str_buf_len] = '\0';
    const char *c_str = c_str_buf;

    /* 3. Save old state for immutability checks */
    struct aws_byte_cursor old_cursor = cursor;
    struct store_byte_from_buffer old_cursor_storage;
    if (cursor.ptr != NULL && cursor.len > 0) {
        save_byte_from_array(cursor.ptr, cursor.len, &old_cursor_storage);
    }
    struct store_byte_from_buffer old_c_str_storage;
    save_byte_from_array((uint8_t *)c_str, c_str_buf_len + 1, &old_c_str_storage);

    /* 4. Call function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Assert postconditions */

    /* the cursor struct itself must remain unchanged */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);

    /* the buffer contents pointed to by cursor must remain unchanged */
    if (cursor.ptr != NULL && cursor.len > 0) {
        assert_byte_from_buffer_matches(cursor.ptr, &old_cursor_storage);
    }

    /* the c_str buffer must remain unchanged */
    assert_byte_from_buffer_matches((uint8_t *)c_str, &old_c_str_storage);

    /* the result must correctly reflect the equality of cursor data and the null-terminated string */
    size_t c_str_len = strlen(c_str);
    bool expected = (cursor.len == c_str_len) && (memcmp(cursor.ptr, c_str, cursor.len) == 0);
    assert(result == expected);

    /* validity invariant still holds (cursor is unchanged, so it's still valid) */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* free allocated memory */
    free(cursor.ptr);
    free(c_str_buf);
}
