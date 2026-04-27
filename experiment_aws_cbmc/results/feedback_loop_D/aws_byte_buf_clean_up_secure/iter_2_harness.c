#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"
#include <aws/common/memory.h>

void aws_byte_buf_clean_up_secure_harness() {
    struct aws_byte_buf buf;
    struct aws_byte_buf old_buf;

    // Use a default allocator to avoid issues with function pointer dereferences
    struct aws_allocator *allocator = aws_default_allocator();

    // Initialize buf with arbitrary values
    buf.len = nondet_size_t();
    buf.buffer = can_fail_malloc(buf.len);
    buf.capacity = nondet_size_t();
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
