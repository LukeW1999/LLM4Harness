#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_byte_cursor_harness() {
    struct aws_string str;
    struct aws_byte_cursor cur;

    // Initialize str
    str.allocator = (struct aws_allocator *)any_ptr();
    str.len = nd_size_t();
    str.bytes = (const uint8_t *)any_ptr();

    // Initialize cur
    cur.ptr = (uint8_t *)any_ptr();
    cur.len = nd_size_t();

    // Save old state
    struct aws_string old_str = str;
    struct aws_byte_cursor old_cur = cur;

    // Ensure cur is bounded
    assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));

    // Ensure str is valid
    assume(aws_string_is_valid(&str));

    // Call the function under test
    bool result = aws_string_eq_byte_cursor(&str, &cur);

    // Assertions for frame conditions and validity invariants
    assert(str.allocator == old_str.allocator);
    assert(str.len == old_str.len);
    assert(str.bytes == old_str.bytes);
    assert(cur.ptr == old_cur.ptr);
    assert(cur.len == old_cur.len);
    assert(aws_string_is_valid(&str));
    assert(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));

    // Postcondition: if lengths differ, result should be false
    if (str.len != cur.len) {
        assert(!result);
    }

    // Postcondition: if lengths are the same, result depends on byte comparison
    if (str.len == cur.len) {
        bool expected_result = true;
        for (size_t i = 0; i < str.len; i++) {
            if (str.bytes[i] != cur.ptr[i]) {
                expected_result = false;
                break;
            }
        }
        assert(result == expected_result);
    }
}
