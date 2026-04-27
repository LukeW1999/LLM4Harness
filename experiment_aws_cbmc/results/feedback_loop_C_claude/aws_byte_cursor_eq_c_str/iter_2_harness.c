#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

void aws_byte_cursor_eq_c_str_harness(void) {
    /* Allocate and initialize a non-deterministic aws_byte_cursor */
    struct aws_byte_cursor cursor;
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Bound the cursor size to keep verification tractable */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, 8));

    /* Allocate a non-deterministic null-terminated C string */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= 8);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    c_str[c_str_len] = '\0';

    /* Save old state before the call */
    struct aws_byte_cursor old_cursor = cursor;
    const char *old_c_str_ptr = c_str;

    /* Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* === FRAME CONDITIONS: cursor must be unchanged === */
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.len == old_cursor.len);

    /* === VALIDITY INVARIANTS === */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* === c_str pointer unchanged === */
    assert(c_str == old_c_str_ptr);
}
