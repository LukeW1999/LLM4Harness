#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_advance_harness() {
    struct aws_byte_cursor cursor;
    size_t len = nondet_size_t();

    // Ensure the cursor is bounded and valid
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    // Ensure len is within the bounds of the cursor's length
    __CPROVER_assume(len <= cursor.len);

    // Save old state
    struct aws_byte_cursor old_cursor = cursor;

    // Call the function
    bool result = aws_byte_cursor_advance(&cursor, len);

    // Assertions for success path
    if (result) {
        assert(cursor.ptr == old_cursor.ptr + len);
        assert(cursor.len == old_cursor.len - len);
    } else {
        // Assertions for failure path
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    // Frame conditions
    assert(aws_byte_cursor_is_valid(&cursor));
}
