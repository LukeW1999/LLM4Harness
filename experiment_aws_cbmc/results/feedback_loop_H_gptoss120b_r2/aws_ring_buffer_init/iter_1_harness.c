#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness(void) {
    /* 1. Declare the ring buffer and allocator */
    struct aws_ring_buffer ring_buf;
    struct aws_ring_buffer old;
    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Non-deterministic size, bounded to a reasonable range */
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size <= 1024); /* arbitrary bound for proof */

    /* 3. Assume the ring buffer is initially valid (even though it will be zeroed) */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 4. Save old state before the call */
    old = ring_buf;

    /* 5. Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* allocation must be non‑NULL and exactly the requested size */
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocation_end == (uint8_t *)ring_buf.allocation + size);

        /* allocator must be the one we passed */
        assert(ring_buf.allocator == allocator);

        /* head and tail atomic pointers must point to the start of the allocation */
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
    } else {
        /* on failure the ring buffer remains empty (all pointers NULL) */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(ring_buf.allocator == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
    }

    /* 7. Fields that must not change regardless of result (none besides those already checked) */

    /* 8. Ring buffer must remain valid after the call */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
