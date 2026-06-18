#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_acquire_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_ring_buffer ring_buf = {0};

    size_t allocation_size;
    __CPROVER_assume(allocation_size > 0);
    __CPROVER_assume(allocation_size <= MAX_BUFFER_SIZE);

    if (aws_ring_buffer_init(&ring_buf, allocator, allocation_size) != AWS_OP_SUCCESS) {
        return;
    }

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_byte_buf dest;
    AWS_ZERO_STRUCT(dest);

    size_t requested_size;
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    struct aws_allocator *old_allocator = ring_buf.allocator;
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    void *old_head = aws_atomic_load_ptr(&ring_buf.head);
    void *old_tail = aws_atomic_load_ptr(&ring_buf.tail);

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    void *new_head = aws_atomic_load_ptr(&ring_buf.head);
    void *new_tail = aws_atomic_load_ptr(&ring_buf.tail);

    __CPROVER_assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR, "aws_ring_buffer_acquire returns success or error");

    __CPROVER_assert(ring_buf.allocator == old_allocator, "allocator is unchanged");
    __CPROVER_assert(ring_buf.allocation == old_allocation, "allocation is unchanged");
    __CPROVER_assert(ring_buf.allocation_end == old_allocation_end, "allocation_end is unchanged");
    __CPROVER_assert(new_tail == old_tail, "tail is unchanged");

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(dest.allocator == NULL, "acquired byte buf has no allocator");
        __CPROVER_assert(dest.len == 0, "acquired byte buf length is zero");
        __CPROVER_assert(dest.capacity == requested_size, "acquired byte buf capacity matches requested size");
        __CPROVER_assert(dest.buffer != NULL, "acquired byte buf has a buffer");
        __CPROVER_assert(dest.buffer >= ring_buf.allocation, "acquired byte buf starts in ring buffer allocation");
        __CPROVER_assert(dest.buffer + dest.capacity <= ring_buf.allocation_end, "acquired byte buf ends in ring buffer allocation");
    } else {
        __CPROVER_assert(new_head == old_head, "head is unchanged on failure");
    }

    __CPROVER_assert(aws_ring_buffer_is_valid(&ring_buf), "ring buffer remains valid");
    __CPROVER_assert(aws_byte_buf_is_valid(&dest), "destination byte buf is valid");
    __CPROVER_assert(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE), "destination byte buf is bounded");

    aws_ring_buffer_clean_up(&ring_buf);
}
