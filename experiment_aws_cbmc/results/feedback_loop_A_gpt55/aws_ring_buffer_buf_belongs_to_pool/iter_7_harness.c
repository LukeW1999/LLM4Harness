#include <proof_helpers/make_common_data_structures.h>

struct aws_allocator *aws_default_allocator(void);

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_ring_buffer ring_buffer;
    ensure_ring_buffer_has_allocated_members(&ring_buffer);
    ring_buffer.allocator = allocator;

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    __CPROVER_assume(ring_buffer.allocation != NULL);
    __CPROVER_assume(
        __CPROVER_POINTER_OBJECT(ring_buffer.allocation) == __CPROVER_POINTER_OBJECT(ring_buffer.allocation_end));
    __CPROVER_assume(
        __CPROVER_POINTER_OFFSET(ring_buffer.allocation) < __CPROVER_POINTER_OFFSET(ring_buffer.allocation_end));

    struct aws_byte_buf buf;
    buf.allocator = allocator;
    buf.buffer = ring_buffer.allocation;
    buf.len = 0;
    buf.capacity = 1;

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_allocator *old_ring_allocator = ring_buffer.allocator;
    uint8_t *old_ring_allocation = ring_buffer.allocation;
    uint8_t *old_ring_allocation_end = ring_buffer.allocation_end;
    uint8_t *old_ring_head = ring_buffer.head;
    uint8_t *old_ring_tail = ring_buffer.tail;

    struct aws_allocator *old_buf_allocator = buf.allocator;
    uint8_t *old_buf_buffer = buf.buffer;
    size_t old_buf_len = buf.len;
    size_t old_buf_capacity = buf.capacity;

    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    __CPROVER_assert(result, "buf belongs to ring buffer pool");

    __CPROVER_assert(ring_buffer.allocator == old_ring_allocator, "ring_buffer allocator unchanged");
    __CPROVER_assert(ring_buffer.allocation == old_ring_allocation, "ring_buffer allocation unchanged");
    __CPROVER_assert(ring_buffer.allocation_end == old_ring_allocation_end, "ring_buffer allocation_end unchanged");
    __CPROVER_assert(ring_buffer.head == old_ring_head, "ring_buffer head unchanged");
    __CPROVER_assert(ring_buffer.tail == old_ring_tail, "ring_buffer tail unchanged");

    __CPROVER_assert(buf.allocator == old_buf_allocator, "buf allocator unchanged");
    __CPROVER_assert(buf.buffer == old_buf_buffer, "buf buffer unchanged");
    __CPROVER_assert(buf.len == old_buf_len, "buf len unchanged");
    __CPROVER_assert(buf.capacity == old_buf_capacity, "buf capacity unchanged");

    __CPROVER_assert(aws_ring_buffer_is_valid(&ring_buffer), "ring_buffer remains valid");
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "buf remains valid");
}
