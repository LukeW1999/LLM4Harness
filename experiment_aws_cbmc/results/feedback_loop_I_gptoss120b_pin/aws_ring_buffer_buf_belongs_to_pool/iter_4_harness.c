#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    struct aws_ring_buffer ring_buf = {0};
    struct aws_byte_buf buf = {0};

    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;

    ring_buf.allocation = (uint8_t *)allocator->mem_acquire(allocator, ring_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + ring_size;

    size_t head = nondet_size_t();
    __CPROVER_assume(head <= ring_size);
    size_t tail = nondet_size_t();
    __CPROVER_assume(tail <= ring_size);
    ring_buf.head = (struct aws_atomic_var){ .value = head };
    ring_buf.tail = (struct aws_atomic_var){ .value = tail };

    size_t offset = nondet_size_t();
    __CPROVER_assume(offset <= ring_size);
    buf.buffer = ring_buf.allocation + offset;
    size_t max_len = (size_t)(ring_buf.allocation_end - buf.buffer);
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= max_len);
    buf.capacity = max_len;
    buf.allocator = allocator;

    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);
    (void)result;
}
