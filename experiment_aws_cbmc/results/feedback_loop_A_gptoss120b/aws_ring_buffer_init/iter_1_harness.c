#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>
#include "cbmc_proof.h"

void aws_ring_buffer_init_harness(void) {
    /* 1. Declare inputs */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = (struct aws_allocator *)nondet_uint8_t();
    size_t size = nondet_size_t();

    /* 2. Preconditions */
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* 3. Save old state */
    struct aws_ring_buffer old = ring_buf;

    /* 4. Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 5. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* allocation must be non‑NULL */
        assert(ring_buf.allocation != NULL);
        /* allocator must be stored */
        assert(ring_buf.allocator == allocator);
        /* allocation_end must be allocation + size */
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
        /* head and tail atomic pointers must equal allocation */
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
    } else {
        /* on failure the struct remains zeroed */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
    }

    /* 6. Fields that never change (none beyond those above) */
    /* No additional unchanged fields to assert */

    /* 7. Validity invariant must always hold */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
