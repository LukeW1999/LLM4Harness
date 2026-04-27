#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_advance_harness() {
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len;

    // Initialize buffer and output with arbitrary values
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    output.buffer = NULL;
    output.len = 0;
    output.capacity = 0;
    output.allocator = NULL;

    // Ensure buffer is valid
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));
    __CPROVER_assume(len <= buffer.capacity); // Ensure len does not exceed capacity

    // Save old state
    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output = output;

    bool result = aws_byte_buf_advance(&buffer, &output, len);

    // Frame conditions and validity invariants
    if (result) {
        // Success path assertions
        assert(buffer.len == old_buffer.len + len);
        assert(output.buffer == old_buffer.buffer + old_buffer.len);
        assert(output.len == 0);
        assert(output.capacity == len);
        assert(output.allocator == NULL);
        assert(aws_byte_buf_is_valid(&buffer));
        assert(aws_byte_buf_is_valid(&output));
    } else {
        // Failure path assertions
        assert(buffer.len == old_buffer.len);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(output.buffer == old_output.buffer);
        assert(output.len == old_output.len);
        assert(output.capacity == old_output.capacity);
        assert(output.allocator == old_output.allocator);
        assert(aws_byte_buf_is_valid(&buffer));
        // output is not guaranteed to be valid on failure
    }
}
