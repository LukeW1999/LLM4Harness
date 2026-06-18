#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

void aws_ring_buffer_clean_up_harness(void) {
    /* Stack‑allocate the ring buffer structure */
    struct aws_ring_buffer ring_buf;
    memset(&ring_buf, 0, sizeof(ring_buf));

    /* Set a valid allocator */
    ring_buf.allocator = aws_default_allocator();

    /* Non‑deterministically decide whether the buffer has an allocation */
    bool has_allocation = nondet_bool();
    if (has_allocation) {
        /* Choose a non‑zero size for the allocation */
        size_t alloc_size = nondet_size_t();
        __CPROVER_assume(alloc_size > 0);

        /* Allocate the underlying memory */
        ring_buf.allocation = aws_mem_acquire(ring_buf.allocator, alloc_size);
        __CPROVER_assume(ring_buf.allocation != NULL);

        /* Set the allocation end pointer */
        ring_buf.allocation_end = (uint8_t *)ring_buf.allocation + alloc_size;

        /* Initialize head and tail atomic variables to zero */
        aws_atomic_store_int(&ring_buf.head, 0);
        aws_atomic_store_int(&ring_buf.tail, 0);
    } else {
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
        aws_atomic_store_int(&ring_buf.head, 0);
        aws_atomic_store_int(&ring_buf.tail, 0);
    }

    /* Assume the ring buffer satisfies its internal validity predicate */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Capture a copy of the original allocation pointer for later checks */
    uint8_t *original_allocation = ring_buf.allocation;

    /* Call the function under verification */
    aws_ring_buffer_clean_up(&ring_buf);

    /* Post‑condition: all fields must be zero */
    struct aws_ring_buffer zeroed;
    memset(&zeroed, 0, sizeof(zeroed));
    assert(memcmp(&ring_buf, &zeroed, sizeof(ring_buf)) == 0);

    /* If there was an allocation, it must have been released */
    if (original_allocation != NULL) {
        assert(ring_buf.allocation == NULL);
    }
}
