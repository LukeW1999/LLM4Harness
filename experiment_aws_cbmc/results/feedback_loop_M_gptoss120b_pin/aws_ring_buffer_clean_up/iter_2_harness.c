#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;

    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size <= 1024);

    bool has_allocation = nondet_bool();
    if (has_allocation) {
        ring_buf.allocation = (uint8_t *)malloc(alloc_size);
        __CPROVER_assume(ring_buf.allocation != NULL);
        ring_buf.allocation_end = ring_buf.allocation + alloc_size;
    } else {
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
    }

    size_t head_val = nondet_size_t();
    size_t tail_val = nondet_size_t();
    if (ring_buf.allocation) {
        __CPROVER_assume(head_val <= alloc_size);
        __CPROVER_assume(tail_val <= alloc_size);
    } else {
        __CPROVER_assume(head_val == 0);
        __CPROVER_assume(tail_val == 0);
    }
    aws_atomic_store_int(&ring_buf.head, (int)head_val);
    aws_atomic_store_int(&ring_buf.tail, (int)tail_val);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    aws_ring_buffer_clean_up(&ring_buf);

    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(aws_atomic_load_int(&ring_buf.head) == 0);
    assert(aws_atomic_load_int(&ring_buf.tail) == 0);
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
