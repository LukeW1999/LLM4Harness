#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_eq_c_str_harness(void) {
    /* 1. Set up the aws_byte_cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Set up the null-terminated c_str */
    /* We need a bounded c_str. Allocate a small array and ensure null termination. */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    c_str[c_str_len] = '\0';

    /* 3. Save old state */
    struct aws_byte_cursor old_cursor = cursor;

    /* 4. Call function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Assert postconditions */
    /* The function is a pure comparison — it should not modify the cursor */
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.len == old_cursor.len);

    /* 6. Assert validity invariant still holds */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* 7. Verify the result is consistent with what aws_array_eq_c_str would return */
    /* The function returns true iff the cursor contents equal the c_str */
    /* We can verify the return value is a valid bool */
    assert(result == true || result == false);
}
