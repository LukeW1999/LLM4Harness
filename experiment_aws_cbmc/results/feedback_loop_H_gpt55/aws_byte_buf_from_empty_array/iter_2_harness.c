#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_empty_array_harness() {
    struct aws_byte_buf array;
    __CPROVER_assume(aws_byte_buf_is_bounded(&array, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&array);
    __CPROVER_assume(aws_byte_buf_is_valid(&array));

    struct store_byte_from_buffer old_byte;
    if (array.capacity > 0) {
        save_byte_from_array(array.buffer, array.capacity, &old_byte);
    }

    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(array.buffer, array.capacity);

    assert(buf.buffer == array.buffer);
    assert(buf.len == 0);
    assert(buf.capacity == array.capacity);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));

    if (array.capacity > 0) {
        assert_byte_from_buffer_matches(array.buffer, &old_byte);
    }
}
