#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_reset_harness() {
    /* Data structures */
    struct aws_byte_buf *buffer = malloc(sizeof(*buffer));

    /* Preconditions */
    __CPROVER_assume(buffer != NULL);
    __CPROVER_assume(aws_byte_buf_is_bounded(buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(buffer));

    /* Save old buffer state */
    struct aws_byte_buf old_buffer = *buffer;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(buffer->buffer, buffer->len, &old_byte);

    /* Non-deterministic choice for zero_contents */
    bool zero_contents;

    /* Call the function */
    aws_byte_buf_reset(buffer, zero_contents);

    /* Postconditions */
    assert(aws_byte_buf_is_valid(buffer));
    assert(buffer->len == 0);
    assert(buffer->capacity == old_buffer.capacity);
    assert(buffer->allocator == old_buffer.allocator);
    assert(buffer->buffer == old_buffer.buffer);

    /* If zero_contents is true, first old_buffer.len bytes should be zeroed */
    if (zero_contents && old_buffer.len > 0) {
        for (size_t i = 0; i < old_buffer.len; i++) {
            assert(buffer->buffer[i] == 0);
        }
    } else if (!zero_contents && old_buffer.len > 0) {
        /* If zero_contents is false, the old data should be preserved up to the old length */
        assert_bytes_match(buffer->buffer, old_buffer.buffer, old_buffer.len);
    }
}
