#include <proof_helpers/make_common_data_structures.h>

size_t nondet_size_t();
bool nondet_bool();

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;

    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);

    bool has_allocation = nondet_bool();
    if (has_allocation) {
        ring_buf.allocation = aws_mem_acquire(allocator, alloc_size);
        __CPROVER_assume(ring_buf.allocation != NULL);
        ring_buf.allocation_end = (uint8_t *)ring_buf.allocation + alloc_size;
    } else {
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
    }

    ring_buf.head = (struct aws_atomic_var){ .value = NULL };
    ring_buf.tail = (struct aws_atomic_var){ .value = NULL };

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_ring_buffer old = ring_buf;

    aws_ring_buffer_clean_up(&ring_buf);

    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.head.value == NULL);
    assert(ring_buf.tail.value == NULL);
}
