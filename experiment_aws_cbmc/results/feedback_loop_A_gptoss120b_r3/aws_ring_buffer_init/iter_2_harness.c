#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>          /* for aws_default_allocator() */
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness() {
    /* Allocate the ring buffer structure */
    struct aws_ring_buffer ring_buf;
    /* Ensure the structure is zero‑initialized before the call */
    memset(&ring_buf, 0, sizeof(ring_buf));

    /* Use the default allocator as required */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Nondeterministic size, bounded to keep verification tractable */
    size_t size = nondet_size_t();
    __CPROVER_assume(size <= 1024);   /* arbitrary upper bound */

    /* Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* Post‑condition checks for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Allocation must be non‑NULL and correctly sized */
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        /* Allocator field must be set to the supplied allocator */
        assert(ring_buf.allocator == allocator);

        /* Head and tail atomics must point to the start of the allocation */
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
    } else {
        /* On failure, all fields should remain NULL/zero */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(ring_buf.allocator == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
    }

    /* The ring buffer validity invariant must always hold */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
