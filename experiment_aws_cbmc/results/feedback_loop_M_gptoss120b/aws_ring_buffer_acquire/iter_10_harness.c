#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/memory.h>

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0 && capacity <= 1024);

    ring_buf.allocation = (uint8_t *)aws_mem_acquire(allocator, capacity);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + capacity;
    ring_buf.size = capacity;

    aws_atomic_init_ptr(&ring_buf.head, (void *)ring_buf.allocation);
    aws_atomic_init_ptr(&ring_buf.tail, (void *)ring_buf.allocation);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_byte_buf dest;
    ensure_byte_buf_has_allocated_buffer_member(&dest);

    struct aws_allocator *old_allocator = ring_buf.allocator;
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    size_t old_size = ring_buf.size;
    void *old_head = aws_atomic_load_ptr(&ring_buf.head);
    void *old_tail = aws_atomic_load_ptr(&ring_buf.tail);

    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size > 0 && requested_size <= 1024);

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == requested_size);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
    } else {
        assert(ring_buf.allocator == old_allocator);
        assert(ring_buf.allocation == old_allocation);
        assert(ring_buf.allocation_end == old_allocation_end);
        assert(aws_atomic_load_ptr(&ring_buf.head) == old_head);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == old_tail);
    }

    assert(ring_buf.allocator == old_allocator);
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);
    assert(ring_buf.size == old_size);

    assert(aws_ring_buffer_is_valid(&ring_buf));
}
