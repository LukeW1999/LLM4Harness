#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    AWS_ZERO_STRUCT(ring_buf);

    struct aws_allocator *default_alloc = aws_default_allocator();
    ring_buf.allocator = default_alloc;

    bool has_allocation = nondet_bool();
    if (has_allocation) {
        size_t size = nondet_size_t();
        __CPROVER_assume(size < MAX_BUFFER_SIZE);
        ring_buf.allocation = (uint8_t *)aws_mem_acquire(ring_buf.allocator, size);
        __CPROVER_assume(ring_buf.allocation != NULL);
        ring_buf.allocation_end = ring_buf.allocation + size;
        ring_buf.size = size;
    } else {
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
        ring_buf.size = 0;
    }

    ring_buf.head = (struct aws_atomic_var){ .value = nondet_size_t() };
    ring_buf.tail = (struct aws_atomic_var){ .value = nondet_size_t() };

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_allocator *old_allocator = ring_buf.allocator;

    aws_ring_buffer_clean_up(&ring_buf);

    assert(ring_buf.allocator == old_allocator);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.size == 0);
    assert(ring_buf.head.value == 0);
    assert(ring_buf.tail.value == 0);
}
