#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness() {
    /* 1. Declare ring buffer and allocator */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Non‑deterministic size bounded by the global constant */
    size_t size = nondet_size_t();
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* 3. Save old state (uninitialized, but saved for completeness) */
    struct aws_ring_buffer old = ring_buf;

    /* 4. Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* allocation must be non‑NULL and exactly the requested size */
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocator == allocator);
        /* head and tail must point to the start of the allocation */
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
        /* allocation_end must be allocation + size */
        assert(ring_buf.allocation_end == (uint8_t *)ring_buf.allocation + size);
    } else {
        /* On failure the function zero‑initializes the struct */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
        (void)old; /* silence unused‑variable warning */
    }

    /* 6. Invariant: the ring buffer must always be valid */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
