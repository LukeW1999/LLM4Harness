#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void aws_byte_cursor_eq_c_str_harness(void) {
    /* 1. Set up the aws_byte_cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Set up the c_str (null-terminated string) */
    /* We need a non-deterministic c_str; use a bounded array */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    /* c_str can be NULL only if length is 0, but aws_array_eq_c_str handles NULL c_str */
    /* For safety, let's allow NULL c_str non-deterministically */
    /* Actually, the function passes c_str directly to aws_array_eq_c_str which calls strlen on it */
    /* So c_str must be a valid null-terminated string or NULL is not safe */
    /* We'll ensure c_str is either NULL (handled by aws_array_eq_c_str) or a valid string */
    if (c_str != NULL) {
        /* Make it a valid null-terminated string by setting the last byte to '\0' */
        c_str[c_str_len] = '\0';
        /* The rest of the bytes are non-deterministic (already set by malloc in CBMC) */
    }

    /* 3. Save old state of cursor */
    struct aws_byte_cursor old_cursor = cursor;

    /* 4. Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Assert postconditions */

    /* The function is a pure comparison - it should not modify the cursor */
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.len == old_cursor.len);

    /* The cursor must remain valid after the call */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* The result is a boolean - it's either true or false */
    /* No specific postcondition on the return value beyond it being a bool */
    /* (result is already bool type, so this is implicit) */

    /* If cursor has length 0 and c_str is empty string "", result should be true */
    /* But we can't easily assert this without knowing c_str content */
    /* The key invariant: cursor is unchanged */
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.len == old_cursor.len);
}
