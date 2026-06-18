#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness() {
    struct aws_ring_buffer ring_buffer;
    struct aws_allocator *allocator = aws_default_allocator();

    uint8_t allocation[MAX_BUFFER_SIZE];

    size_t allocation_size = nondet_size_t();
    __CPROVER_assume(allocation_size > 0);
    __CPROVER_assume(allocation_size <= MAX_BUFFER_SIZE);

    ring_buffer.allocator = allocator;
    ring_buffer.allocation = allocation;
    ring_buffer.allocation_end = allocation + allocation_size;

    size_t head_offset = nondet_size_t();
    __CPROVER_assume(head_offset < allocation_size);

    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(tail_offset < allocation_size);

    size_t buf_capacity = nondet_size_t();
    __CPROVER_assume(buf_capacity > 0);
    __CPROVER_assume(buf_capacity <= allocation_size - tail_offset);

    aws_atomic_init_ptr(&ring_buffer.head, ring_buffer.allocation + head_offset);
    aws_atomic_init_ptr(&ring_buffer.tail, ring_buffer.allocation + tail_offset);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    struct aws_byte_buf buf;
    buf.buffer = ring_buffer.allocation + tail_offset;
    buf.capacity = buf_capacity;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = NULL;

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));
    __CPROVER_assume((uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail) == buf.buffer);

    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    uint8_t *old_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);

    struct store_byte_from_buffer old_allocation_byte;
    save_byte_from_array(ring_buffer.allocation, allocation_size, &old_allocation_byte);

    aws_ring_buffer_release(&ring_buffer, &buf);

    uint8_t *expected_tail = old_buf.buffer + old_buf.capacity;
    if (expected_tail == old_ring_buffer.allocation_end) {
        expected_tail = old_ring_buffer.allocation;
    }

    assert((uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail) == expected_tail);
    assert(buf.len == 0);
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    assert((uint8_t *)aws_atomic_load_ptr(&ring_buffer.head) == old_head);
    assert(old_tail == old_buf.buffer);

    assert_byte_from_buffer_matches(ring_buffer.allocation, &old_allocation_byte);

    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));
}
