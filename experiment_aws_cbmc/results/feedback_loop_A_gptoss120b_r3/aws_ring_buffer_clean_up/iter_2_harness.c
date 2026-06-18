#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;

    if (nondet_bool()) {
        size_t alloc_size = nondet_size_t();
        __CPROVER_assume(alloc_size <= 1024);
        uint8_t *alloc = (uint8_t *)nondet_uintptr_t();
        __CPROVER_assume(alloc != NULL);
        ring_buf.allocation = (void *)alloc;
        ring_buf.allocation_end = (void *)(alloc + alloc_size);
    } else {
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
    }

    ring_buf.head = (struct aws_atomic_var){0};
    ring_buf.tail = (struct aws_atomic_var){0};

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_ring_buffer old = ring_buf;

    aws_ring_buffer_clean_up(&ring_buf);

    assert(ring_buf.allocator == allocator);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.head == (struct aws_atomic_var){0});
    assert(ring_buf.tail == (struct aws_atomic_var){0});
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
