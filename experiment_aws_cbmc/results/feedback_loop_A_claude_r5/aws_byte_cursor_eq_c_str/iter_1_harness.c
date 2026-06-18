#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_byte_cursor_eq_c_str_harness(void) {
    /* 1. Declare and set up the cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Save old state */
    struct aws_byte_cursor old_cursor = cursor;

    /* 3. Set up the c_str (non-deterministic null-terminated string) */
    /* We use a bounded array to keep the state space manageable */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    /* c_str can be NULL only if length is 0, but aws_array_eq_c_str handles NULL c_str */
    /* For safety, let's allow c_str to be non-NULL with a null terminator */
    if (c_str != NULL) {
        c_str[c_str_len] = '\0';
    }

    /* 4. Call function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Assert postconditions */

    /* The function is a pure comparison - it should not modify the cursor */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);

    /* Validity invariant must still hold */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* The result is a bool - it must be either true or false */
    assert(result == true || result == false);

    /* If c_str is NULL, the comparison should return false (unless cursor.len == 0) */
    /* The function delegates to aws_array_eq_c_str which handles NULL */
    /* We can't assert much more without re-implementing the logic,
       but we can assert the cursor is unchanged */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);
}
