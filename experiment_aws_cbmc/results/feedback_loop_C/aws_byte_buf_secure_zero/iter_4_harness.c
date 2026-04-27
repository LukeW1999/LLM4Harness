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
    assume(capacity > 0); // Ensure capacity is positive to avoid trivial cases
    buf.buffer = (uint8_t *)malloc(capacity * sizeof(uint8_t));
    assume(buf.buffer); // Ensure buffer is not NULL
    buf.len = nondet_size_t();
    assume(buf.len <= capacity); // Ensure len does not exceed capacity
    buf.capacity = capacity;
    buf.allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    assume(buf.allocator); // Ensure allocator is not NULL

    // Copy initial state to old_buf for comparison later
    old_buf.buffer = buf.buffer;
    old_buf.len = buf.len;
    old_buf.capacity = buf.capacity;
    old_buf.allocator = buf.allocator;

    // Call the function under test
    int result = aws_byte_buf_secure_zero(&buf);

    // Postconditions
    if (result == AWS_OP_SUCCESS) {
        // Success path assertions
        assert(buf.len == 0);
        for (size_t i = 0; i < old_buf.capacity; i++) {
            assert(buf.buffer[i] == 0);
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
