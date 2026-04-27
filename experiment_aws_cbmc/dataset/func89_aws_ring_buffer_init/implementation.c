aws_ring_buffer_init(struct aws_ring_buffer *ring_buf, struct aws_allocator *allocator, size_t size) {
    AWS_PRECONDITION(ring_buf != NULL);
    AWS_PRECONDITION(allocator != NULL);
    AWS_PRECONDITION(size > 0);

    AWS_ZERO_STRUCT(*ring_buf);

    ring_buf->allocation = aws_mem_acquire(allocator, size);

    if (!ring_buf->allocation) {
        return AWS_OP_ERR;
    }

    ring_buf->allocator = allocator;
    aws_atomic_init_ptr(&ring_buf->head, ring_buf->allocation);
    aws_atomic_init_ptr(&ring_buf->tail, ring_buf->allocation);
    ring_buf->allocation_end = ring_buf->allocation + size;

    AWS_POSTCONDITION(aws_ring_buffer_is_valid(ring_buf));
    return AWS_OP_SUCCESS;
}