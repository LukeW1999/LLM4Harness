#include <assert.h>
#include <aws/common/ring_buffer.h>
#include "make_common_data_structures.h"

void aws_ring_buffer_init_harness() {
    /* 1. Declare data structures */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator;
    __CPROVER_assume(allocator != NULL);

    size_t size = nondet_size_t();
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* 2. Save old state */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 4. Postconditions for both success and failure */
    if (result == AWS_OP_SUCCESS) {
        /* Changed fields on success */
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation_end == (uint8_t *)ring_buf.allocation + size);
        /* Validity invariant */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* Fields on failure (allocation failed) */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation_end == NULL);
        /* Validity invariant (still must hold) */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }

    /* 5. Fields that must not change regardless of result (none in this struct) */

    /* 6. Overall validity invariant */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
