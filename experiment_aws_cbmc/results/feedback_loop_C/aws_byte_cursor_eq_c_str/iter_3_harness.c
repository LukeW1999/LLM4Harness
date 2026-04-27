#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_eq_c_str_harness() {
    struct aws_byte_cursor cursor;
    const char *c_str;

    // Initialize cursor and c_str
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    c_str = bounded_string(c_str_len);

    // Ensure c_str is not NULL
    __CPROVER_assume(c_str != NULL);

    // Ensure cursor is valid
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    // Save old state
    struct aws_byte_cursor old_cursor = cursor;
    const char *old_c_str = c_str;

    // Call the function under test
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    // Assert frame conditions
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.len == old_cursor.len);
    assert(c_str == old_c_str);

    // Assert validity invariants
    assert(aws_byte_cursor_is_valid(&cursor));
}
