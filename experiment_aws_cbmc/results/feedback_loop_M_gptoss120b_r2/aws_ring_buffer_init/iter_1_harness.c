#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_ring_buffer_init_harness() {
    /* 1. Declare the ring buffer and allocator */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Non‑deterministic size bounded by the global constant */
    size_t size = nondet_size_t();
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* 3. Save old state (not strictly needed but kept for completeness) */
    struct aws_ring_buffer old = ring_buf;

    /* 4. Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* allocation must be non‑NULL */
        assert(ring_buf.allocation != NULL);
        /* allocation_end must be exactly allocation + size */
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
        /* allocator field must be the one we passed */
        assert(ring_buf.allocator == allocator);
        /* head and tail atomic pointers must point to the allocation */
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
    } else {
        /* on failure the struct should remain zeroed */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(ring_buf.allocator == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
    }

    /* 6. Invariant: the ring buffer must always be valid */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
