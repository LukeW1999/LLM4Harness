#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old_ring_buf = ring_buf;

    /* 3. Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        assert(ring_buf.allocator == old_ring_buf.allocator);
        assert(ring_buf.allocation == old_ring_buf.allocation);
        assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(ring_buf.head == old_ring_buf.head);
    assert(ring_buf.tail == old_ring_buf.tail);

    /* 6. Assert validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
