#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

void aws_byte_buf_reset_harness() {
    struct aws_byte_buf buf;
    bool zero_contents = nondet_bool();

    // Initialize buf with arbitrary values
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0 && capacity <= 1024); // Reasonable capacity to avoid excessive memory usage
    buf.buffer = (uint8_t *)malloc(sizeof(uint8_t) * capacity);
    __CPROVER_assume(buf.buffer != NULL); // Ensure buffer is allocated
    buf.len = nondet_size_t();
    buf.capacity = capacity;
    buf.allocator = aws_default_allocator(); // Use default allocator

    // Save old state
    struct aws_byte_buf old_buf = buf;

    // Call the function
    aws_byte_buf_reset(&buf, zero_contents);

    // Assertions for success path
    assert(buf.len == 0);
    if (zero_contents) {
        for (size_t i = 0; i < old_buf.len; i++) { // Only check up to old length for zeroing
            assert(buf.buffer[i] == 0);
        }
    } else {
        assert(buf.buffer == old_buf.buffer);
    }

    // Assertions for frame conditions
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    // Validity invariant
    assert(aws_byte_buf_is_valid(&buf));

    // Free allocated memory
    free(old_buf.buffer);
}
