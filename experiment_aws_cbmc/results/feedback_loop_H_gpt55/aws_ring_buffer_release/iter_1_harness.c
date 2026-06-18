#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness() {
    struct aws_ring_buffer ring_buffer;

    size_t allocation_size = nondet_size_t();
    __CPROVER_assume(allocation_size > 0);
    __CPROVER_assume(allocation_size <= MAX_BUFFER_SIZE);

    ring_buffer.allocator = aws_default_allocator();
    ring_buffer.allocation = malloc(allocation_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + allocation_size;

    size_t head_offset = nondet_size_t();
    __CPROVER_assume(head_offset <= allocation_size);

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

    assert((uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail) == old_buf.buffer + old_buf.capacity);
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
