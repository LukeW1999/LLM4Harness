#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_reset_harness(void) {
    struct aws_byte_buf buf;

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    bool zero_contents;

    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(buf.buffer, buf.capacity, &old_byte);

    aws_byte_buf_reset(&buf, zero_contents);

    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.len == 0);

    if (zero_contents && old_buf.buffer != NULL && old_buf.capacity > 0) {
        size_t index;
        __CPROVER_assume(index < old_buf.capacity);
        assert(buf.buffer[index] == 0);
    }

    if (!zero_contents) {
        assert_byte_from_buffer_matches(buf.buffer, &old_byte);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
