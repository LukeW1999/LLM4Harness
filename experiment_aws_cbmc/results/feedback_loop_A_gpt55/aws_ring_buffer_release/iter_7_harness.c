#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    __CPROVER_assume(allocator != NULL);

    ensure_ring_buffer_has_allocated_members(&ring_buffer, MAX_BUFFER_SIZE);
    ring_buffer.allocator = allocator;

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    uint8_t *tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    size_t capacity = nondet_size_t();
    size_t len = nondet_size_t();

    __CPROVER_assume(tail >= ring_buffer.allocation);
    __CPROVER_assume(tail < ring_buffer.allocation_end);
    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(capacity <= (size_t)(ring_buffer.allocation_end - tail));
    __CPROVER_assume(len <= capacity);

    buf.allocator = NULL;
    buf.buffer = tail;
    buf.len = len;
    buf.capacity = capacity;

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));
    __CPROVER_assume((uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail) == buf.buffer);

    uint8_t *old_buf_buffer = buf.buffer;
    size_t old_buf_capacity = buf.capacity;
    struct aws_allocator *old_ring_allocator = ring_buffer.allocator;
    uint8_t *old_ring_allocation = ring_buffer.allocation;
    uint8_t *old_ring_allocation_end = ring_buffer.allocation_end;
    uint8_t *old_ring_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);

    uint8_t *expected_tail = old_buf_buffer + old_buf_capacity;
    if (expected_tail == old_ring_allocation_end) {
        expected_tail = old_ring_allocation;
    }

    aws_ring_buffer_release(&ring_buffer, &buf);

    __CPROVER_assert(
        (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail) == expected_tail,
        "tail is advanced to end of released buffer");

    __CPROVER_assert(buf.allocator == NULL, "released buffer allocator is NULL");
    __CPROVER_assert(buf.buffer == NULL, "released buffer pointer is NULL");
    __CPROVER_assert(buf.len == 0, "released buffer len is zero");
    __CPROVER_assert(buf.capacity == 0, "released buffer capacity is zero");

    __CPROVER_assert(ring_buffer.allocator == old_ring_allocator, "ring buffer allocator is unchanged");
    __CPROVER_assert(ring_buffer.allocation == old_ring_allocation, "ring buffer allocation is unchanged");
    __CPROVER_assert(
        (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head) == old_ring_head,
        "ring buffer head is unchanged");
    __CPROVER_assert(ring_buffer.allocation_end == old_ring_allocation_end, "ring buffer allocation end is unchanged");

    __CPROVER_assert(aws_ring_buffer_is_valid(&ring_buffer), "ring buffer is valid after release");
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "byte buffer is valid after release");
}
