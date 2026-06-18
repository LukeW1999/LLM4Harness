#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness() {
    /* Non-deterministic inputs */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size;

    /* Preconditions: size must be valid for allocation */
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* Save old state */
    struct aws_ring_buffer old_ring_buf = ring_buf;

    /* Call function */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: allocation is non-null and within bounds */
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
        assert(ring_buf.allocator == allocator);
        /* head and tail point to start of allocation */
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
        /* allocation_end is within bounds */
        assert(ring_buf.allocation_end > ring_buf.allocation);
    } else {
        /* Failure: struct is zeroed */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
    }

    /* Unchanged fields: none (all fields are initialized or zeroed) */
    /* Validity invariant: ring buffer is valid after success */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }
}
