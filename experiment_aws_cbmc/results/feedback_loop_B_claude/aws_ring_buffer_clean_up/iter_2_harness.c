#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* Allocate a buffer so the ring buffer is valid */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0 && alloc_size <= 4096);

    ring_buf.allocator = allocator;
    ring_buf.allocation = (uint8_t *)aws_mem_acquire(allocator, alloc_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + alloc_size;

    /* head and tail must be within [allocation, allocation_end] for validity */
    uint8_t *head_ptr = nondet_uint8_t_ptr();
    uint8_t *tail_ptr = nondet_uint8_t_ptr();
    __CPROVER_assume(head_ptr >= ring_buf.allocation && head_ptr <= ring_buf.allocation_end);
    __CPROVER_assume(tail_ptr >= ring_buf.allocation && tail_ptr <= ring_buf.allocation_end);

    aws_atomic_init_ptr(&ring_buf.head, (void *)head_ptr);
    aws_atomic_init_ptr(&ring_buf.tail, (void *)tail_ptr);

    /* Ensure the ring buffer is valid before calling clean_up */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* Assert postconditions: AWS_ZERO_STRUCT zeroes everything */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);

    void *head_val = aws_atomic_load_ptr(&ring_buf.head);
    void *tail_val = aws_atomic_load_ptr(&ring_buf.tail);
    assert(head_val == NULL);
    assert(tail_val == NULL);
}
