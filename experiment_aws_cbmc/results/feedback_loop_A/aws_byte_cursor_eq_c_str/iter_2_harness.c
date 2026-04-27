#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_eq_c_str_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    const char *c_str;
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, cursor.len + 1)); // +1 for null terminator

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_cursor old_cursor = cursor;

    /* 3. Call function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    // For this function, there are no fields that change, so we only need to check immutability

    /* 5. Assert fields that must NOT change regardless of result */
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.len == old_cursor.len);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_cursor_is_valid(&cursor));
}
