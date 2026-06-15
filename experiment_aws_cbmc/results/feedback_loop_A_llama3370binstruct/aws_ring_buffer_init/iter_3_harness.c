#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size = nondet_size_t();

    /* 2. Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 3. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation!= NULL);
        assert(aws_ring_buffer_is_valid(&ring_buf));
        assert(ring_buf.capacity == size);
    } else {
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation == NULL);
    }

    /* 4. Assert fields that must NOT change regardless of result */
    assert(result == AWS_OP_SUCCESS || result == AWS_ERROR_INVALID_SIZE);

    /* 5. Assert validity invariant always holds */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }
}

int main() {
    aws_ring_buffer_init_harness();
    return 0;
}
