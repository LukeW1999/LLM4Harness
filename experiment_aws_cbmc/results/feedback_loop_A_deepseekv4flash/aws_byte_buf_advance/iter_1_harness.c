#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_advance_harness() {
    /* nondet inputs */
    struct aws_byte_buf *buffer = malloc(sizeof(*buffer));
    struct aws_byte_buf *output = malloc(sizeof(*output));
    size_t len;

    /* preconditions: buffers non-null */
    __CPROVER_assume(buffer != NULL && output != NULL);
    __CPROVER_assume(buffer != output); /* restrict says no aliasing */
    __CPROVER_assume(aws_byte_buf_is_bounded(buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(buffer));

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
        /* output not necessarily valid, but it's zeroed, so we can assert its fields */
    }
}
