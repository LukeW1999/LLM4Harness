#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>

void aws_ring_buffer_init_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size;
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* Save old state */
    struct aws_ring_buffer old = ring_buf;

    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    if (result == AWS_OP_SUCCESS) {
        /* Allocation must be non‑NULL */
        assert(ring_buf.allocation != NULL);
        /* Allocator must be the one passed in */
        assert(ring_buf.allocator == allocator);
        /* Head and tail must point to the start of the allocation */
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
        /* allocation_end must be allocation + size */
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
    } else {
        /* On failure all fields remain zero / NULL */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
        assert(ring_buf.allocation_end == NULL);
    }

    /* Validity invariant holds exactly on success */
    assert(aws_ring_buffer_is_valid(&ring_buf) == (result == AWS_OP_SUCCESS));
}
