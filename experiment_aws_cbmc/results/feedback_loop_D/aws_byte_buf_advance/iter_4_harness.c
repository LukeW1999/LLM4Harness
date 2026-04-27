#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_advance_harness() {
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len;

    // Initialize buffer with non-deterministic values
    buffer.len = nondet_size_t();
    buffer.capacity = nondet_size_t();
    buffer.allocator = aws_default_allocator();
    __CPROVER_assume(buffer.len <= buffer.capacity);
    buffer.buffer = can_fail_malloc(buffer.capacity);
    __CPROVER_assume(buffer.buffer != NULL);

    // Initialize output with non-deterministic values
    output.buffer = NULL;
    output.len = 0;
    output.capacity = 0;
    output.allocator = NULL;

    // Save old state of buffer
    struct aws_byte_buf old_buffer = buffer;

    // Add assumptions for len
    __CPROVER_assume(len <= buffer.len);

    // Precondition checks for aws_byte_buf_advance
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    // Call the function under test
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    // Assertions for success path
    if (result) {
        assert(buffer.len == old_buffer.len - len);
        assert(output.buffer == old_buffer.buffer);
        assert(output.len == len);
        assert(output.capacity == len);
        assert(output.allocator == NULL);
    } else {
        // Assertions for failure path
        assert(buffer.len == old_buffer.len);
        assert(output.buffer == NULL);
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.allocator == NULL);
    }

    // Validity invariants
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));

    // Free allocated memory
    free(buffer.buffer);
}
