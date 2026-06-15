#include <aws/common/ring_buffer.h>
#include <assert.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_ring_buffer_init_harness(void) {
    /* 1. Declare data structures */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = nondet_pointer();
    size_t size = nondet_size_t();

    /* 2. Pre‑condition assumptions */
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* 3. Save old state */
    struct aws_ring_buffer old = ring_buf;

    /* 4. Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* allocation must be non‑NULL */
        assert(ring_buf.allocation != NULL);
        /* allocator must be the one passed in */
        assert(ring_buf.allocator == allocator);
        /* allocation_end must be allocation + size */
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
        /* head and tail atomic pointers must point inside the buffer */
        assert(aws_ring_buffer_check_atomic_ptr(&ring_buf, ring_buf.allocation));
        /* the whole structure must be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* on failure the allocation is NULL */
        assert(ring_buf.allocation == NULL);
        /* allocator remains NULL (zeroed by AWS_ZERO_STRUCT) */
        assert(ring_buf.allocator == NULL);
        /* allocation_end is also NULL */
        assert(ring_buf.allocation_end == NULL);
        /* the structure is not a valid, usable ring buffer */
        assert(!aws_ring_buffer_is_valid(&ring_buf));
    }

    /* 6. Fields that never change regardless of outcome */
    /* (All fields are either set on success or zeroed on failure, so no additional unchanged‑field asserts) */
}
