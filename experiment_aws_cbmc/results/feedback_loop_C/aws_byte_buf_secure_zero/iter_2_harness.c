#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>

void aws_byte_buf_secure_zero_harness() {
    struct aws_byte_buf buf;
    struct aws_byte_buf old_buf;

    // Initialize buf with arbitrary values
    size_t capacity = nondet_size_t();
    buf.buffer = (uint8_t *)malloc(capacity * sizeof(uint8_t));
    buf.len = nondet_size_t();
    buf.capacity = capacity;
    buf.allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));

    // Copy initial state to old_buf for comparison later
    old_buf.buffer = buf.buffer;
    old_buf.len = buf.len;
    old_buf.capacity = buf.capacity;
    old_buf.allocator = buf.allocator;

    // Ensure the buffer is bounded to prevent CBMC from exploring invalid states
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    // Call the function under test
    int result = aws_byte_buf_secure_zero(&buf);

    // Postconditions
    if (result == AWS_OP_SUCCESS) {
        // Success path assertions
        assert(buf.len == 0);
        if (old_buf.buffer != NULL && old_buf.capacity > 0) {
            for (size_t i = 0; i < old_buf.capacity; i++) {
                assert(buf.buffer[i] == 0);
            }
        }
    } else {
        // Failure path assertions
        assert(buf.buffer == old_buf.buffer);
        assert(buf.len == old_buf.len);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.allocator == old_buf.allocator);
    }

    // Validity invariant
    assert(aws_byte_buf_is_valid(&buf));

    // Free allocated memory
    free(buf.buffer);
    free(buf.allocator);
}
