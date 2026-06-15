#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_advance_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len;

    /* Assumptions on buffer validity and boundedness */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

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
}
