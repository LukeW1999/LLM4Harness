#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#   define MAX_BUFFER_SIZE 1024
#endif

void aws_ring_buffer_init_harness(void) {
    /* Allocate a ring buffer structure */
    struct aws_ring_buffer ring_buf;
    /* Ensure the structure is zero‑initialized before the call */
    AWS_ZERO_STRUCT(ring_buf);

    /* Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Nondeterministic size bounded by MAX_BUFFER_SIZE */
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* Save a copy of the pre‑state for fields that must not change on failure */
    struct aws_ring_buffer old = ring_buf;

    /* Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Allocation must be non‑NULL and exactly the requested size */
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocation_end == (uint8_t *)ring_buf.allocation + size);

        /* Allocator must be the one we passed in */
        assert(ring_buf.allocator == allocator);

        /* Head and tail atomic pointers must both point to the start of the allocation */
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);

        /* Size field must be set to the requested size */
        assert(ring_buf.size == size);

        /* Validity invariant must hold */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* On failure the structure should be zeroed (AWS_ZERO_STRUCT applied) */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(ring_buf.allocator == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
        assert(ring_buf.size == 0);

        /* Fields that must not change on failure */
        assert(ring_buf.allocation == old.allocation);
        assert(ring_buf.allocation_end == old.allocation_end);
        assert(ring_buf.allocator == old.allocator);
        assert(aws_atomic_load_ptr(&ring_buf.head) == aws_atomic_load_ptr(&old.head));
        assert(aws_atomic_load_ptr(&ring_buf.tail) == aws_atomic_load_ptr(&old.tail));
        assert(ring_buf.size == old.size);
    }
}
