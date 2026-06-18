#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <stddef.h>

void aws_byte_buf_advance_harness(void) {
    /* Non-deterministic inputs */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len;

    /* Assume preconditions */
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* Save old state of buffer */
    struct aws_byte_buf old_buffer = buffer;

    /* Call function */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* Postconditions */

    /* 1. Check buffer validity */
    assert(aws_byte_buf_is_valid(&buffer));

    /* 2. Check output validity */
    assert(aws_byte_buf_is_valid(&output));

    if (result) {
        /* Success path */
        /* buffer->len increased by len */
        assert(buffer.len == old_buffer.len + len);
        /* buffer->capacity unchanged */
        assert(buffer.capacity == old_buffer.capacity);
        /* buffer->allocator unchanged */
        assert(buffer.allocator == old_buffer.allocator);
        /* buffer->buffer unchanged (pointer not changed) */
        assert(buffer.buffer == old_buffer.buffer);

        /* output properties */
        if (old_buffer.buffer == NULL) {
            assert(output.buffer == NULL);
        } else {
            assert(output.buffer == old_buffer.buffer + old_buffer.len);
        }
        assert(output.len == 0);
        assert(output.capacity == len);
        assert(output.allocator == NULL);
    } else {
        /* Failure path */
        /* buffer unchanged */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.buffer == old_buffer.buffer);

        /* output zeroed */
        assert(output.len == 0);
        assert(output.buffer == NULL);
        assert(output.capacity == 0);
        assert(output.allocator == NULL);
    }

    return 0;
}
