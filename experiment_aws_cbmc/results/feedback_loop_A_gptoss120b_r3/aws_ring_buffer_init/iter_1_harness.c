#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>          /* for aws_default_allocator() */
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>

void aws_ring_buffer_init_harness() {
    /* 1. Declare the ring buffer and inputs */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t size = nondet_size_t();
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* 2. Save old state (for completeness, though init overwrites) */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 4. Post‑condition checks for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* allocation must be non‑NULL and correctly sized */
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        /* allocator field must be set to the supplied allocator */
        assert(ring_buf.allocator == allocator);

        /* head and tail atomics must point to the start of the allocation */
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
    } else {
        /* allocation failed – all fields should remain zeroed */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(ring_buf.allocator == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
    }

    /* 5. Unchanged fields – the struct has no other fields to check */

    /* 6. Validity invariant must always hold */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
