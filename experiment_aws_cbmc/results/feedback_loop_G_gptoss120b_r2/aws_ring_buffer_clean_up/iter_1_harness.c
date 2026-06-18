#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>          /* for aws_default_allocator() */
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>

/* The harness verifies the behaviour of aws_ring_buffer_clean_up. */
void aws_ring_buffer_clean_up_harness(void) {
    /* 1. Declare the ring buffer and nondeterministically initialise it. */
    struct aws_ring_buffer ring_buf;

    /* Allocator must be a valid allocator – use the default allocator. */
    ring_buf.allocator = aws_default_allocator();

    /* Non‑deterministically decide whether the buffer has an allocation. */
    uint8_t *alloc = nondet_uint8_t() ? NULL : (uint8_t *)malloc(1);
    ring_buf.allocation = alloc;

    if (alloc != NULL) {
        /* Choose a non‑deterministic size for the allocation, bounded by the
         * proof‑time constant MAX_BUFFER_SIZE. */
        size_t size = nondet_size_t();
        __CPROVER_assume(size <= MAX_BUFFER_SIZE);
        ring_buf.allocation_end = alloc + size;
    } else {
        ring_buf.allocation_end = NULL;
    }

    /* Initialise the atomic head and tail with nondeterministic values. */
    ring_buf.head.value = nondet_size_t();
    ring_buf.tail.value = nondet_size_t();

    /* Assume the ring buffer is valid before the clean‑up. */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save the old state for later comparison. */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call the function under test. */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 4. Post‑conditions: on success (the function always succeeds) all fields
     *    must be zeroed. */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.head.value == 0);
    assert(ring_buf.tail.value == 0);

    /* 5. No fields other than those listed above exist, so no additional
     *    unchanged‑field assertions are required. */

    /* 6. The ring buffer after clean‑up is intentionally zeroed; it is not
     *    required to satisfy aws_ring_buffer_is_valid().  The only invariant
     *    we must guarantee is that the structure has been cleared as shown
     *    above. */
}
