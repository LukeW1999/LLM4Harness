#include <assert.h>
#include <stddef.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include "make_common_data_structures.h"

void aws_ring_buffer_init_harness(void) {
    /* 1. Allocate a dummy allocator (its internals are not exercised here) */
    struct aws_allocator allocator_obj;
    struct aws_allocator *allocator = &allocator_obj;
    __CPROVER_assume(allocator != NULL);

    /* 2. Choose a nondeterministic size (including zero to force failure) */
    size_t size = nondet_size_t();
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* 3. Declare the ring buffer and remember its initial state */
    struct aws_ring_buffer ring_buf;
    struct aws_ring_buffer old = ring_buf;

    /* 4. Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 5. Post‑conditions for the success case */
    if (result == AWS_OP_SUCCESS) {
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation_end == (uint8_t *)ring_buf.allocation + size);
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* 6. Post‑conditions for the failure case (e.g., size == 0 or allocation failure) */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }

    /* 7. Global invariant that must always hold */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
