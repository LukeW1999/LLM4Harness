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

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring_buffer;

    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    ring_buffer.allocator = allocator;
    ring_buffer.allocation = malloc(ring_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + ring_size;

    struct aws_byte_buf buf;

    size_t buf_offset;
    __CPROVER_assume(buf_offset < ring_size);
    buf.buffer = ring_buffer.allocation + buf_offset;

    size_t buf_capacity;
    __CPROVER_assume(buf_capacity > 0 && buf_capacity <= ring_size - buf_offset);
    buf.capacity = buf_capacity;

    __CPROVER_assume(buf.buffer + buf.capacity <= ring_buffer.allocation_end);

    size_t buf_len;
    __CPROVER_assume(buf_len <= buf_capacity);
    buf.len = buf_len;

    buf.allocator = NULL;

    /* tail must equal buf.buffer for release to be valid */
    aws_atomic_init_ptr(&ring_buffer.tail, (void *)buf.buffer);

    /* head must be set such that the ring buffer is valid */
    uint8_t *head_ptr;
    __CPROVER_assume(head_ptr >= ring_buffer.allocation && head_ptr <= ring_buffer.allocation_end);
    aws_atomic_init_ptr(&ring_buffer.head, (void *)head_ptr);

    /* Ensure all validity conditions hold */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    /* Compute expected new tail after release */
    uint8_t *raw_new_tail = buf.buffer + buf.capacity;
    uint8_t *expected_new_tail;
    if (raw_new_tail == ring_buffer.allocation_end) {
        expected_new_tail = ring_buffer.allocation;
    } else {
        expected_new_tail = raw_new_tail;
    }

    struct aws_allocator *old_allocator = ring_buffer.allocator;
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);

    aws_ring_buffer_release(&ring_buffer, &buf);

    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    assert(new_tail == expected_new_tail);

    assert(ring_buffer.allocator == old_allocator);
    assert(ring_buffer.allocation == old_allocation);
    assert(ring_buffer.allocation_end == old_allocation_end);

    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    assert(new_head == old_head);
}
