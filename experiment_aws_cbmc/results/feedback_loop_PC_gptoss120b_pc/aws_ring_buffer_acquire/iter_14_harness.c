#include <proof_helpers/make_common_data_structures.h>

size_t nondet_size_t(void);
bool nondet_bool(void);

#define MAX_BUFFER_SIZE 256

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf dest;

    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;

    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0);
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);
    ring_buf.allocation_size = alloc_size;

    ring_buf.allocation = (uint8_t *)allocator->mem_acquire(allocator, alloc_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + alloc_size;

    bool empty = nondet_bool();
    if (empty) {
        aws_atomic_store_ptr(&ring_buf.head, ring_buf.allocation);
        aws_atomic_store_ptr(&ring_buf.tail, ring_buf.allocation);
    } else {
        uint8_t *head_ptr = ring_buf.allocation + (nondet_size_t() % alloc_size);
        uint8_t *tail_ptr = ring_buf.allocation + (nondet_size_t() % alloc_size);
        __CPROVER_assume(head_ptr != tail_ptr);
        aws_atomic_store_ptr(&ring_buf.head, head_ptr);
        aws_atomic_store_ptr(&ring_buf.tail, tail_ptr);
    }

    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *old_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
    struct aws_byte_buf old_dest = dest;

    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
        assert(dest.len == requested_size);
        assert(dest.capacity == requested_size);
        assert(aws_byte_buf_is_valid(&dest));

        size_t diff;
        if (new_head >= old_head) {
            diff = (size_t)(new_head - old_head);
        } else {
            diff = (size_t)(ring_buf.allocation_end - old_head) + (size_t)(new_head - ring_buf.allocation);
        }
        assert(diff == requested_size);
        assert(new_tail == old_tail);
    } else {
        assert(new_head == old_head);
        assert(new_tail == old_tail);
        assert(ring_buf.allocation == ring_buf.allocation);
        assert(ring_buf.allocation_end == ring_buf.allocation_end);
        assert(ring_buf.allocator == allocator);

        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
    }

    assert(ring_buf.allocation == ring_buf.allocation);
    assert(ring_buf.allocation_end == ring_buf.allocation_end);
    assert(ring_buf.allocator == allocator);
    assert(ring_buf.allocation_size == alloc_size);

    assert(aws_ring_buffer_is_valid(&ring_buf));
}
