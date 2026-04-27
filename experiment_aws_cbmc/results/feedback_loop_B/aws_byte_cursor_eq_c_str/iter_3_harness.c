#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_eq_c_str_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    char c_str[MAX_BUFFER_SIZE];
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, MAX_BUFFER_SIZE));
    __CPROVER_assume(strlen(c_str) <= MAX_BUFFER_SIZE - 1); // Ensure c_str is null-terminated

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_cursor old_cursor = cursor;
    char old_c_str[MAX_BUFFER_SIZE];
    memcpy(old_c_str, c_str, MAX_BUFFER_SIZE);

    /* 3. Call function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        assert(aws_array_eq_c_str(cursor.ptr, cursor.len, c_str));
    } else {
        /* No changes to cursor or c_str on failure */
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
        assert(memcmp(c_str, old_c_str, MAX_BUFFER_SIZE) == 0);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* No fields in cursor or c_str are modified by this function */

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_cursor_is_valid(&cursor));
}
