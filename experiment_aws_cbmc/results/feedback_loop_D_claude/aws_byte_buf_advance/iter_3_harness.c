#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_byte_buf_advance_harness(void) {
    /* Set up buffer */
    struct aws_byte_buf buffer;
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));
    /* Ensure buffer.buffer is non-NULL so pointer arithmetic is defined */
    __CPROVER_assume(buffer.buffer != NULL);

    /* Set up output - must be valid before call (precondition) */
    struct aws_byte_buf output;
    ensure_byte_buf_has_allocated_buffer_member(&output);
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* Non-deterministic len */
    size_t len;

    /* Save old state */
    struct aws_byte_buf old_buffer = buffer;

    /* Call the function */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* === Verify frame conditions for buffer === */
    assert(buffer.capacity == old_buffer.capacity);
    assert(buffer.buffer == old_buffer.buffer);
    assert(buffer.allocator == old_buffer.allocator);

    if (result) {
        /* === SUCCESS PATH === */
        assert(buffer.len == old_buffer.len + len);

        assert(output.len == 0);
        assert(output.capacity == len);
        assert(output.allocator == NULL);

        /* output->buffer points to correct location */
        assert(output.buffer == old_buffer.buffer + old_buffer.len);

        assert(aws_byte_buf_is_valid(&buffer));
        assert(aws_byte_buf_is_valid(&output));

    } else {
        /* === FAILURE PATH === */
        assert(buffer.len == old_buffer.len);

        assert(output.len == 0);
        assert(output.buffer == NULL);
        assert(output.capacity == 0);
        assert(output.allocator == NULL);

        assert(aws_byte_buf_is_valid(&buffer));
        assert(aws_byte_buf_is_valid(&output));
    }
}
