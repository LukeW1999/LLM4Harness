#include <aws/common/byte_buf.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_advance_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len;

    /* Allocate buffer's backing memory */
    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
    uint8_t *data = malloc(capacity);
    buffer.buffer = data;
    buffer.capacity = capacity;
    buffer.len = nondet_size_t();
    __CPROVER_assume(buffer.len <= buffer.capacity);
    buffer.allocator = nondet_bool() ? (aws_allocator *)malloc(sizeof(aws_allocator)) : NULL;

    /* Ensure output is valid (function will overwrite it) */
    output.buffer = NULL;
    output.capacity = 0;
    output.len = 0;
    output.allocator = NULL;

    /* Save old state */
    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output = output;

    /* Call the function */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* Postconditions */
    if (result) {
        /* Success: buffer->len increased by len, output is a sub-buffer */
        assert(buffer.len == old_buffer.len + len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.buffer == old_buffer.buffer);
        /* output is a view into buffer's old end */
        assert(output.len == 0);
        assert(output.capacity == len);
        if (old_buffer.buffer != NULL) {
            assert(output.buffer == old_buffer.buffer + old_buffer.len);
        } else {
            assert(output.buffer == NULL);
        }
        assert(output.allocator == NULL);
    } else {
        /* Failure: buffer unchanged, output zeroed */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.buffer == old_buffer.buffer);
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.buffer == NULL);
        assert(output.allocator == NULL);
    }

    /* Validity invariants */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));

    /* Clean up */
    free(data);
    if (old_buffer.allocator != NULL) free(old_buffer.allocator);
}
