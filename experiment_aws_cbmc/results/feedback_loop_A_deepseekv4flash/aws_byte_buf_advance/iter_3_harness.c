#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_advance_harness() {
    struct aws_byte_buf *buffer = malloc(sizeof(*buffer));
    struct aws_byte_buf *output = malloc(sizeof(*output));
    size_t len;

    /* preconditions: buffers non-null and non-aliasing */
    __CPROVER_assume(buffer != NULL && output != NULL);
    __CPROVER_assume(buffer != output);

    /* ensure buffer has allocated buffer and is valid */
    ensure_byte_buf_has_allocated_buffer_member(buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(buffer));

    /* nondeterministically choose whether len fits in buffer's remaining capacity */
    bool success_possible;
    __CPROVER_assume(success_possible); // nondet
    if (success_possible) {
        __CPROVER_assume(len <= buffer->capacity - buffer->len);
    } else {
        __CPROVER_assume(len > buffer->capacity - buffer->len);
    }

    /* Save old state */
    struct aws_byte_buf old_buffer = *buffer;
    struct aws_byte_buf old_output = *output;

    /* call function */
    bool result = aws_byte_buf_advance(buffer, output, len);

    /* postconditions */
    if (result) {
        /* success path */
        assert(buffer->len == old_buffer.len + len);
        assert(buffer->capacity == old_buffer.capacity);
        assert(buffer->allocator == old_buffer.allocator);
        assert(buffer->buffer == old_buffer.buffer);
        /* output fields */
        assert(output->len == 0);
        assert(output->capacity == len);
        assert(output->allocator == NULL);
        assert(output->buffer == (old_buffer.buffer == NULL ? NULL : old_buffer.buffer + old_buffer.len));
        assert(aws_byte_buf_is_valid(buffer));
        assert(aws_byte_buf_is_valid(output));
    } else {
        /* failure path */
        /* buffer unchanged */
        assert(buffer->len == old_buffer.len);
        assert(buffer->capacity == old_buffer.capacity);
        assert(buffer->allocator == old_buffer.allocator);
        assert(buffer->buffer == old_buffer.buffer);
        /* output zeroed */
        assert(output->len == 0);
        assert(output->capacity == 0);
        assert(output->buffer == NULL);
        assert(output->allocator == NULL);
        assert(aws_byte_buf_is_valid(buffer));
    }
}
