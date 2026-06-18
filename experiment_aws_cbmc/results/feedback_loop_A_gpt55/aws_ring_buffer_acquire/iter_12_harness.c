#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_acquire_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_ring_buffer ring_buf;
    ensure_ring_buffer_has_allocated_members(&ring_buf);
    ring_buf.allocator = allocator;

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    size_t capacity = aws_ring_buffer_buf_size(&ring_buf);
    __CPROVER_assume(capacity > 1);
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    size_t requested_size;
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size < capacity);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    struct aws_byte_buf dest;

    struct aws_allocator *old_allocator = ring_buf.allocator;
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *old_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);

    __CPROVER_assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR, "result is a valid aws status");
    __CPROVER_assert(ring_buf.allocator == old_allocator, "allocator is unchanged");
    __CPROVER_assert(ring_buf.allocation == old_allocation, "allocation is unchanged");
    __CPROVER_assert(ring_buf.allocation_end == old_allocation_end, "allocation_end is unchanged");
    __CPROVER_assert(new_tail == old_tail, "tail is unchanged");
    __CPROVER_assert(aws_ring_buffer_is_valid(&ring_buf), "ring buffer remains valid");

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(dest.len == 0, "acquired buffer length is zero");
        __CPROVER_assert(dest.capacity == requested_size, "acquired buffer capacity is requested size");
        __CPROVER_assert(dest.allocator == NULL, "acquired buffer allocator is NULL");
        __CPROVER_assert(dest.buffer != NULL, "acquired buffer is non-NULL");
        __CPROVER_assert(aws_byte_buf_is_valid(&dest), "acquired byte buffer is valid");
    } else {
        __CPROVER_assert(new_head == old_head, "head is unchanged on failure");
    }
}
