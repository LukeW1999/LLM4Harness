#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void aws_byte_cursor_eq_c_str_harness(void) {
    /* 1. Declare and bound the cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);

    /* nondet length, bounded by the allocated buffer size */
    cursor.len = nondet_size_t();
    __CPROVER_assume(cursor.len <= MAX_BUFFER_SIZE);

    /* 2. Assume the cursor is valid */
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 3. Allocate a nondet C string */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len < MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* fill with nondet bytes */
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str[i] = nondet_uint8_t();
    }
    c_str[c_str_len] = '\0';

    /* 4. Save old state */
    struct aws_byte_cursor old_cursor = cursor;
    char *old_c_str = c_str;
    struct store_byte_from_buffer old_buf_storage;
    save_byte_from_array(cursor.ptr, cursor.len, &old_buf_storage);

    /* 5. Compute expected result using the underlying helper */
    bool expected = aws_array_eq_c_str(cursor.ptr, cursor.len, c_str);

    /* 6. Call function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 7. Post‑condition: result must match the helper */
    assert(result == expected);

    /* 8. Unchanged fields */
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.len == old_cursor.len);
    assert(c_str == old_c_str);
    assert_byte_from_buffer_matches(cursor.ptr, &old_buf_storage);

    /* 9. Validity invariant */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* clean up */
    free(c_str);
}
