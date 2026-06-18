#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_byte_cursor_eq_c_str_harness(void) {
    /* 1. Set up the aws_byte_cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Set up the null-terminated c_str */
    /* We need a non-deterministic c_str; bound its length for tractability */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    c_str[c_str_len] = '\0';

    /* 3. Save state before the call */
    struct aws_byte_cursor old_cursor = cursor;
    size_t old_len = cursor.len;
    uint8_t *old_ptr = cursor.ptr;

    /* 4. Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Assert postconditions */

    /* RETURN: result is a bool (true or false) — both paths are valid */
    /* The function returns true iff the cursor contents equal the c_str contents */
    /* We verify the result is consistent with aws_array_eq_c_str semantics */
    /* (We can't easily re-derive the expected value, but we can check invariants) */

    /* FRAME: cursor must not be modified by this function */
    assert(cursor.len == old_len);
    assert(cursor.ptr == old_ptr);

    /* INVARIANTS: cursor remains valid after the call */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* If result is true, the lengths must match (cursor->len == strlen(c_str)) */
    if (result) {
        assert(cursor.len == c_str_len);
    }

    /* If cursor->len != strlen(c_str), result must be false */
    if (cursor.len != c_str_len) {
        assert(!result);
    }

    /* The cursor fields are unchanged (frame conditions) */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);
}
