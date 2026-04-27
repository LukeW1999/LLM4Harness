#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_clean_up_secure_harness() {
    struct aws_byte_buf buf;
    struct aws_byte_buf old_buf;

    // Use a default allocator to avoid issues with function pointer dereferences
    struct aws_allocator *allocator = aws_default_allocator();

    // Initialize buf with arbitrary values
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    buf.buffer = can_fail_malloc(len);
    if (buf.buffer == NULL) {
        len = 0;
    }
    buf.len = len;
    buf.capacity = len; // Set capacity to len to ensure buffer is valid
    buf.allocator = allocator;

    // Store the old values for comparison
    old_buf = buf;

    // Ensure the byte buffer is bounded and valid
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    // Call the function under test
    aws_byte_buf_clean_up_secure(&buf);

    // Assertions based on the analysis
    if (old_buf.buffer != NULL) {
        // If the buffer was not NULL, it should have been freed
        assert(buf.buffer == NULL);
    }

    // Check the frame conditions
    if (buf.buffer == NULL) {
        // On success path
        assert(buf.len == 0);
        assert(buf.capacity == 0);
    } else {
        // On failure path
        assert(buf.len == old_buf.len);
        assert(buf.buffer == old_buf.buffer);
        assert(buf.capacity == old_buf.capacity);
    }

    // Allocator should remain unchanged
    assert(buf.allocator == old_buf.allocator);

    // Check the validity invariant
    assert(aws_byte_buf_is_valid(&buf));
}
