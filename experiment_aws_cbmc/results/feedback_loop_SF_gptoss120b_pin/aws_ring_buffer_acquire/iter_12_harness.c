#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_ring_buffer *rb = &ring_buf;

    struct aws_allocator *allocator = aws_default_allocator();
    rb->allocator = allocator;

    /* Allocate a static buffer for the ring buffer */
    uint8_t static_buffer[1024];
    size_t alloc_size = __CPROVER_nondet_size_t();
    __CPROVER_assume(alloc_size > 0);
    __CPROVER_assume(alloc_size <= 1024);
    uint8_t *alloc = static_buffer;
    rb->allocation = alloc;
    rb->allocation_end = alloc + alloc_size;
    rb->size = alloc_size;

    /* Choose head and tail offsets */
    size_t head_offset = __CPROVER_nondet_size_t();
    __CPROVER_assume(head_offset < alloc_size);
    uint8_t *head_ptr = alloc + head_offset;

    size_t tail_offset = __CPROVER_nondet_size_t();
    __CPROVER_assume(tail_offset < alloc_size);
    uint8_t *tail_ptr = alloc + tail_offset;

    /* Ensure head is not before tail (valid state) */
    __CPROVER_assume(head_offset >= tail_offset);

    rb->head = (uintptr_t)head_ptr;
    rb->tail = (uintptr_t)tail_ptr;

    __CPROVER_assume(aws_ring_buffer_is_valid(rb));

    /* Prepare destination byte buffer */
    struct aws_byte_buf dest;
    struct aws_byte_buf *dest_ptr = &dest;
    dest.buffer = NULL;
    dest.len = 0;
    dest.capacity = 0;
    dest.allocator = allocator;
    __CPROVER_assume(aws_byte_buf_is_valid(dest_ptr));

    /* Choose a requested size that fits */
    size_t requested_size = __CPROVER_nondet_size_t();
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size <= 1024);
    __CPROVER_assume(requested_size <= alloc_size - (head_offset - tail_offset));
    __CPROVER_assume(requested_size <= alloc_size - head_offset);

    uint8_t *old_head = (uint8_t *)rb->head;
    uint8_t *old_tail = (uint8_t *)rb->tail;
    size_t old_alloc_size = alloc_size;
    struct aws_byte_buf old_dest = *dest_ptr;

    int result = aws_ring_buffer_acquire(rb, requested_size, dest_ptr);

    /* Post‑conditions */
    assert(aws_ring_buffer_is_valid(rb));
    assert(rb->allocation_end - rb->allocation == old_alloc_size);
    assert((uint8_t *)rb->tail == old_tail);

    if (result == 0) {
        assert(dest_ptr->buffer == old_head);
        assert(dest_ptr->len == requested_size);
        assert(dest_ptr->capacity == requested_size);
        assert(aws_byte_buf_is_valid(dest_ptr));

        uint8_t *new_head = (uint8_t *)rb->head;
        assert(new_head >= rb->allocation);
        assert(new_head <= rb->allocation_end);
        assert(new_head != old_head);
    } else {
        assert(dest_ptr->buffer == old_dest.buffer);
        assert(dest_ptr->len == old_dest.len);
        assert(dest_ptr->capacity == old_dest.capacity);
        assert(aws_byte_buf_is_valid(dest_ptr));
        assert((uint8_t *)rb->head == old_head);
    }
}
