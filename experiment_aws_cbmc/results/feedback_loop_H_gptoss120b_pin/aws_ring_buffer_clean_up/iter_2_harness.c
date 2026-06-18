#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness(void) {
    /* Declare and zero-initialize the ring buffer */
    struct aws_ring_buffer ring_buf;
    AWS_ZERO_STRUCT(ring_buf);

    /* Use the default allocator */
    struct aws_allocator *default_alloc = aws_default_allocator();
    ring_buf.allocator = default_alloc;

    /* Nondeterministically decide whether the buffer has an allocation */
    bool has_allocation = nondet_bool();
    if (has_allocation) {
        size_t size = nondet_size_t();
        __CPROVER_assume(size < MAX_BUFFER_SIZE);
        ring_buf.allocation = aws_mem_acquire(ring_buf.allocator, size);
        __CPROVER_assume(ring_buf.allocation != NULL);
        ring_buf.allocation_end = ring_buf.allocation + size;
        ring_buf.size = size;
    } else {
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
        ring_buf.size = 0;
    }

    /* Initialize head and tail with nondeterministic values */
    ring_buf.head = (struct aws_atomic_var){ .value = nondet_uint64_t() };
    ring_buf.tail = (struct aws_atomic_var){ .value = nondet_uint64_t() };

    /* Assume the ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Save old allocator for later comparison */
    struct aws_allocator *old_allocator = ring_buf.allocator;

    /* Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* Post‑conditions */
    assert(ring_buf.allocator == old_allocator);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.size == 0);
    assert(ring_buf.head.value == 0);
    assert(ring_buf.tail.value == 0);
}
