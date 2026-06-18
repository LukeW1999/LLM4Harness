#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_ring_buffer ring_buffer;
    ensure_ring_buffer_has_allocated_members(&ring_buffer);
    ring_buffer.allocator = allocator;

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    size_t allocation_size = (size_t)(ring_buffer.allocation_end - ring_buffer.allocation);
    __CPROVER_assume(allocation_size > 0);

    struct aws_byte_buf buf;

    size_t buf_offset = nondet_size_t();
    __CPROVER_assume(buf_offset < allocation_size);

    size_t buf_capacity = nondet_size_t();
    __CPROVER_assume(buf_capacity > 0);
    __CPROVER_assume(buf_capacity <= allocation_size - buf_offset);

    size_t buf_len = nondet_size_t();
    __CPROVER_assume(buf_len <= buf_capacity);

    buf.allocator = allocator;
    buf.buffer = ring_buffer.allocation + buf_offset;
    buf.len = buf_len;
    buf.capacity = buf_capacity;

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_allocator *old_ring_allocator = ring_buffer.allocator;
    uint8_t *old_ring_allocation = ring_buffer.allocation;
    uint8_t *old_ring_allocation_end = ring_buffer.allocation_end;
    uint8_t *old_ring_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    uint8_t *old_ring_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);

    struct aws_allocator *old_buf_allocator = buf.allocator;
    uint8_t *old_buf_buffer = buf.buffer;
    size_t old_buf_len = buf.len;
    size_t old_buf_capacity = buf.capacity;

    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    bool expected = (buf.buffer >= ring_buffer.allocation) && (buf.buffer < ring_buffer.allocation_end) &&
                    (buf.buffer + buf.capacity <= ring_buffer.allocation_end);

    __CPROVER_assert(result == expected, "aws_ring_buffer_buf_belongs_to_pool returns expected result");

    __CPROVER_assert(ring_buffer.allocator == old_ring_allocator, "ring_buffer allocator unchanged");
    __CPROVER_assert(ring_buffer.allocation == old_ring_allocation, "ring_buffer allocation unchanged");
    __CPROVER_assert(ring_buffer.allocation_end == old_ring_allocation_end, "ring_buffer allocation_end unchanged");
    __CPROVER_assert((uint8_t *)aws_atomic_load_ptr(&ring_buffer.head) == old_ring_head, "ring_buffer head unchanged");
    __CPROVER_assert((uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail) == old_ring_tail, "ring_buffer tail unchanged");

    __CPROVER_assert(buf.allocator == old_buf_allocator, "buf allocator unchanged");
    __CPROVER_assert(buf.buffer == old_buf_buffer, "buf buffer unchanged");
    __CPROVER_assert(buf.len == old_buf_len, "buf len unchanged");
    __CPROVER_assert(buf.capacity == old_buf_capacity, "buf capacity unchanged");

    __CPROVER_assert(aws_ring_buffer_is_valid(&ring_buffer), "ring_buffer remains valid");
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "buf remains valid");
}
