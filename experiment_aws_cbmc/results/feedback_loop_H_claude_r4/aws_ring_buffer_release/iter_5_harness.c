#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef MAX_BUFFER_SIZE
#    define MAX_BUFFER_SIZE 64
#endif

void aws_ring_buffer_release_harness() {
    struct aws_ring_buffer ring_buffer;

    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    ring_buffer.allocator = allocator;
    ring_buffer.allocation = (uint8_t *)malloc(ring_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + ring_size;

    /* Set up buf first so we can constrain head/tail around it */
    size_t buf_offset;
    size_t buf_capacity;
    __CPROVER_assume(buf_offset < ring_size);
    __CPROVER_assume(buf_capacity > 0);
    __CPROVER_assume(buf_capacity <= ring_size - buf_offset);

    uint8_t *buf_start = ring_buffer.allocation + buf_offset;
    uint8_t *buf_end = buf_start + buf_capacity;
    __CPROVER_assume(buf_end <= ring_buffer.allocation_end);

    /* head and tail must be valid positions */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= ring_size);
    __CPROVER_assume(tail_offset <= ring_size);

    uint8_t *head_ptr = ring_buffer.allocation + head_offset;
    uint8_t *tail_ptr = ring_buffer.allocation + tail_offset;

    aws_atomic_init_ptr(&ring_buffer.head, (void *)head_ptr);
    aws_atomic_init_ptr(&ring_buffer.tail, (void *)tail_ptr);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* After release, new tail = buf_end. Ensure resulting state is valid. */
    /* We need aws_ring_buffer_is_valid to hold with tail = buf_end */
    /* Temporarily set tail to buf_end and check validity */
    aws_atomic_store_ptr(&ring_buffer.tail, (void *)buf_end);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    /* Restore original tail */
    aws_atomic_store_ptr(&ring_buffer.tail, (void *)tail_ptr);

    struct aws_byte_buf buf;
    buf.buffer = buf_start;
    buf.capacity = buf_capacity;
    size_t buf_len;
    __CPROVER_assume(buf_len <= buf_capacity);
    buf.len = buf_len;
    buf.allocator = ring_buffer.allocator;

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;
    struct aws_allocator *old_allocator = ring_buffer.allocator;
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    uint8_t *expected_tail = buf_end;

    aws_ring_buffer_release(&ring_buffer, &buf);

    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    assert(new_tail == expected_tail);

    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    assert(ring_buffer.allocator == old_allocator);
    assert(ring_buffer.allocation == old_allocation);
    assert(ring_buffer.allocation_end == old_allocation_end);

    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    assert(new_head == old_head);

    assert(new_tail >= ring_buffer.allocation);
    assert(new_tail <= ring_buffer.allocation_end);
}
