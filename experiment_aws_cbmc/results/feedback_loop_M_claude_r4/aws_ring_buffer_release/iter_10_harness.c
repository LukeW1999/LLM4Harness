#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

void aws_ring_buffer_release_harness() {
    struct aws_ring_buffer ring_buffer;

    size_t ring_size;
    __CPROVER_assume(ring_size > 1 && ring_size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    ring_buffer.allocator = allocator;
    ring_buffer.allocation = malloc(ring_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + ring_size;

    size_t tail_offset;
    __CPROVER_assume(tail_offset < ring_size);
    uint8_t *tail_ptr = ring_buffer.allocation + tail_offset;
    aws_atomic_store_ptr(&ring_buffer.tail, (void *)tail_ptr);

    size_t head_offset;
    __CPROVER_assume(head_offset < ring_size);
    uint8_t *head_ptr = ring_buffer.allocation + head_offset;
    aws_atomic_store_ptr(&ring_buffer.head, (void *)head_ptr);

    /* Set up the byte buf to release */
    struct aws_byte_buf buf;

    /* buf.buffer must equal the current tail */
    buf.buffer = tail_ptr;

    /* buf.capacity must be such that buf.buffer + buf.capacity <= allocation_end - 1 */
    size_t buf_capacity;
    __CPROVER_assume(buf_capacity > 0);
    /* new tail must be strictly less than allocation_end */
    __CPROVER_assume(tail_offset + buf_capacity < ring_size);

    buf.capacity = buf_capacity;

    size_t buf_len;
    __CPROVER_assume(buf_len <= buf.capacity);
    buf.len = buf_len;
    buf.allocator = ring_buffer.allocator;

    /* Ensure the new tail (after release) is within valid range */
    uint8_t *expected_new_tail = buf.buffer + buf.capacity;
    __CPROVER_assume(expected_new_tail >= ring_buffer.allocation);
    __CPROVER_assume(expected_new_tail < ring_buffer.allocation_end);

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    /* Save old state */
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    struct aws_allocator *old_allocator = ring_buffer.allocator;
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;

    /* Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Assert postconditions */
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    assert(new_tail == expected_new_tail);

    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    assert(ring_buffer.allocator == old_allocator);
    assert(ring_buffer.allocation == old_allocation);
    assert(ring_buffer.allocation_end == old_allocation_end);

    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    assert(new_head == old_head);
}
