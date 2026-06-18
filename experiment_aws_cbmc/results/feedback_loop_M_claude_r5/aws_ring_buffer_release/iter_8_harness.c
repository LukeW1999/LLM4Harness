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

    /* Set up tail within allocation */
    size_t tail_offset;
    __CPROVER_assume(tail_offset < ring_size);
    uint8_t *tail_ptr = ring_buffer.allocation + tail_offset;
    aws_atomic_store_ptr(&ring_buffer.tail, (void *)tail_ptr);

    /* buf_capacity must be > 0 and fit from tail to allocation_end */
    size_t buf_capacity;
    __CPROVER_assume(buf_capacity > 0);
    __CPROVER_assume(buf_capacity <= ring_size - tail_offset);

    uint8_t *new_tail = tail_ptr + buf_capacity;
    /* new_tail must be within [allocation, allocation_end] */
    __CPROVER_assume(new_tail >= ring_buffer.allocation);
    __CPROVER_assume(new_tail <= ring_buffer.allocation_end);

    /* Set up head: must be within [allocation, allocation_end] */
    size_t head_offset;
    __CPROVER_assume(head_offset <= ring_size);
    uint8_t *head_ptr = ring_buffer.allocation + head_offset;
    aws_atomic_store_ptr(&ring_buffer.head, (void *)head_ptr);

    /* Ensure ring buffer is valid before call */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Set up the byte buf to release */
    struct aws_byte_buf buf;
    buf.buffer = tail_ptr;
    buf.capacity = buf_capacity;

    size_t buf_len;
    __CPROVER_assume(buf_len <= buf_capacity);
    buf.len = buf_len;
    buf.allocator = ring_buffer.allocator;

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    /* Ensure new_tail satisfies ring buffer validity after release */
    __CPROVER_assume(new_tail >= ring_buffer.allocation && new_tail <= ring_buffer.allocation_end);

    /* Call the function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Assert postconditions */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);
}
