#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_eq_c_str_harness() {
    struct aws_byte_cursor cursor;
    const char *c_str;

    // Initialize cursor with a valid buffer and length
    make_and_initialize_byte_cursor(&cursor, sizeof("test_string"));

    // Set c_str to point to a valid string literal
    c_str = "test_string";

    // Store old values for frame condition checks
    struct aws_byte_cursor old_cursor = cursor;

    // Call the function under test
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    // Frame conditions
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.len == old_cursor.len);

    // Validity invariants
    assert(aws_byte_cursor_is_valid(&cursor));

    // Postconditions based on return value
    if (result) {
        // If true, cursor must match the string exactly
        assert(cursor.len == strlen(c_str));
        assert(memcmp(cursor.ptr, c_str, cursor.len) == 0);
    } else {
        // If false, cursor does not match the string
        assert(cursor.len != strlen(c_str) || memcmp(cursor.ptr, c_str, cursor.len) != 0);
    }
}
