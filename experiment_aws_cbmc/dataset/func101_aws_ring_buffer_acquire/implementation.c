aws_ring_buffer_acquire(struct aws_ring_buffer *ring_buf, size_t requested_size, struct aws_byte_buf *dest) {
    AWS_PRECONDITION(aws_ring_buffer_is_valid(ring_buf));
    AWS_PRECONDITION(aws_byte_buf_is_valid(dest));
    AWS_ERROR_PRECONDITION(requested_size != 0);

    uint8_t *tail_cpy;
    uint8_t *head_cpy;
    AWS_ATOMIC_LOAD_TAIL_PTR(ring_buf, tail_cpy);
    AWS_ATOMIC_LOAD_HEAD_PTR(ring_buf, head_cpy);

    /* this branch is, we don't have any vended buffers. */
    if (head_cpy == tail_cpy) {
        size_t ring_space = ring_buf->allocation_end == NULL ? 0 : ring_buf->allocation_end - ring_buf->allocation;

        if (requested_size > ring_space) {
            AWS_POSTCONDITION(aws_ring_buffer_is_valid(ring_buf));
            AWS_POSTCONDITION(aws_byte_buf_is_valid(dest));
            return aws_raise_error(AWS_ERROR_OOM);
        }
        AWS_ATOMIC_STORE_HEAD_PTR(ring_buf, ring_buf->allocation + requested_size);
        AWS_ATOMIC_STORE_TAIL_PTR(ring_buf, ring_buf->allocation);
        *dest = aws_byte_buf_from_empty_array(ring_buf->allocation, requested_size);
        AWS_POSTCONDITION(aws_ring_buffer_is_valid(ring_buf));
        AWS_POSTCONDITION(aws_byte_buf_is_valid(dest));
        return AWS_OP_SUCCESS;
    }

    /* you'll constantly bounce between the next two branches as the ring buffer is traversed. */
    /* after N + 1 wraps */
    if (tail_cpy > head_cpy) {
        size_t space = tail_cpy - head_cpy - 1;

        if (space >= requested_size) {
            AWS_ATOMIC_STORE_HEAD_PTR(ring_buf, head_cpy + requested_size);
            *dest = aws_byte_buf_from_empty_array(head_cpy, requested_size);
            AWS_POSTCONDITION(aws_ring_buffer_is_valid(ring_buf));
            AWS_POSTCONDITION(aws_byte_buf_is_valid(dest));
            return AWS_OP_SUCCESS;
        }
        /* After N wraps */
    } else if (tail_cpy < head_cpy) {
        /* prefer the head space for efficiency. */
        if ((size_t)(ring_buf->allocation_end - head_cpy) >= requested_size) {
            AWS_ATOMIC_STORE_HEAD_PTR(ring_buf, head_cpy + requested_size);
            *dest = aws_byte_buf_from_empty_array(head_cpy, requested_size);
            AWS_POSTCONDITION(aws_ring_buffer_is_valid(ring_buf));
            AWS_POSTCONDITION(aws_byte_buf_is_valid(dest));
            return AWS_OP_SUCCESS;
        }

        if ((size_t)(tail_cpy - ring_buf->allocation) > requested_size) {
            AWS_ATOMIC_STORE_HEAD_PTR(ring_buf, ring_buf->allocation + requested_size);
            *dest = aws_byte_buf_from_empty_array(ring_buf->allocation, requested_size);
            AWS_POSTCONDITION(aws_ring_buffer_is_valid(ring_buf));
            AWS_POSTCONDITION(aws_byte_buf_is_valid(dest));
            return AWS_OP_SUCCESS;
        }
    }

    AWS_POSTCONDITION(aws_ring_buffer_is_valid(ring_buf));
    AWS_POSTCONDITION(aws_byte_buf_is_valid(dest));
    return aws_raise_error(AWS_ERROR_OOM);
}