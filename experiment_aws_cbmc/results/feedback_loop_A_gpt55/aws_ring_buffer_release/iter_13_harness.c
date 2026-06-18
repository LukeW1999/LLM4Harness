#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    __CPROVER_assume(allocator != NULL);

    size_t ring_buffer_size;
    __CPROVER_assume(ring_buffer_size > 0);
    __CPROVER_assume(ring_buffer_size <= MAX_BUFFER_SIZE);

    ensure_ring_buffer_has_allocated_members(&ring_buffer, ring_buffer_size);
    ring_buffer.allocator = allocator;

    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + ring_buffer_size;

    aws_atomic_store_ptr(&ring_buffer.head, ring_buffer.allocation);

    size_t offset;
    size_t capacity;
    size_t len;

    __CPROVER_assume(offset < ring_buffer_size);
    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(capacity <= ring_buffer_size - offset);
    __CPROVER_assume(len <= capacity);

    uint8_t *tail = ring_buffer.allocation + offset;
    aws_atomic_store_ptr(&ring_buffer.tail, tail);

    buf.allocator = NULL;
    buf.buffer = tail;
    buf.len = len;
    buf.capacity = capacity;

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_atomic_load_ptr(&ring_buffer.tail) == buf.buffer);

    uint8_t *old_buf_buffer = buf.buffer;
    size_t old_buf_capacity = buf.capacity;
    struct aws_allocator *old_ring_allocator = ring_buffer.allocator;
    uint8_t *old_ring_allocation = ring_buffer.allocation;
    uint8_t *old_ring_allocation_end = ring_buffer.allocation_end;
    uint8_t *old_ring_head = aws_atomic_load_ptr(&ring_buffer.head);

    uint8_t *expected_tail;
    if (offset + old_buf_capacity == ring_buffer_size) {
        expected_tail = old_ring_allocation;
    } else {
        expected_tail = old_buf_buffer + old_buf_capacity;
    }

    aws_ring_buffer_release(&ring_buffer, &buf);

    __CPROVER_assert(aws_atomic_load_ptr(&ring_buffer.tail) == expected_tail, "tail is advanced to end of released buffer");

    __CPROVER_assert(buf.allocator == NULL, "released buffer allocator is NULL");
    __CPROVER_assert(buf.buffer == NULL, "released buffer pointer is NULL");
    __CPROVER_assert(buf.len == 0, "released buffer len is zero");
    __CPROVER_assert(buf.capacity == 0, "released buffer capacity is zero");

    __CPROVER_assert(ring_buffer.allocator == old_ring_allocator, "ring buffer allocator is unchanged");
    __CPROVER_assert(ring_buffer.allocation == old_ring_allocation, "ring buffer allocation is unchanged");
    __CPROVER_assert(aws_atomic_load_ptr(&ring_buffer.head) == old_ring_head, "ring buffer head is unchanged");
    __CPROVER_assert(ring_buffer.allocation_end == old_ring_allocation_end, "ring buffer allocation end is unchanged");

    __CPROVER_assert(aws_ring_buffer_is_valid(&ring_buffer), "ring buffer is valid after release");
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "byte buffer is valid after release");
}
