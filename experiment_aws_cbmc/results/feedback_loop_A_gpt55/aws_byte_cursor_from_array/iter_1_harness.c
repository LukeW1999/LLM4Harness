#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_array_harness() {
    struct aws_byte_cursor array;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&array, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&array);
    __CPROVER_assume(aws_byte_cursor_is_valid(&array));

    struct aws_byte_cursor old_array = array;

    struct store_byte_from_buffer old_byte;
    if (array.len > 0) {
        save_byte_from_array(array.ptr, array.len, &old_byte);
    }

    struct aws_byte_cursor result = aws_byte_cursor_from_array(array.ptr, array.len);

    assert(result.ptr == old_array.ptr);
    assert(result.len == old_array.len);

    assert(array.ptr == old_array.ptr);
    assert(array.len == old_array.len);

    if (old_array.len > 0) {
        assert_byte_from_buffer_matches(old_array.ptr, &old_byte);
    }

    assert(aws_byte_cursor_is_valid(&array));
    assert(aws_byte_cursor_is_valid(&result));
    assert(aws_byte_cursor_is_bounded(&result, MAX_BUFFER_SIZE));
}
