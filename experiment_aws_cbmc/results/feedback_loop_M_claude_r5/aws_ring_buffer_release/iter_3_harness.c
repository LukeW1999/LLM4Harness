#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_ring_buffer_release_harness() {
    /* 1. Set up the ring buffer */
    struct aws_ring_buffer ring_buffer;

    /* Allocate the ring buffer's internal allocation */
    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    ring_buffer.allocator = allocator;
    ring_buffer.allocation = malloc(ring_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + ring_size;

    /* Set up head and tail atomics to valid positions within the buffer */
    size_t head_offset;
    __CPROVER_assume(head_offset < ring_size);
    uint8_t *head_ptr = ring_buffer.allocation + head_offset;
    aws_atomic_store_ptr(&ring_buffer.head, (void *)head_ptr);

    size_t tail_offset;
    __CPROVER_assume(tail_offset < ring_size);
    uint8_t *tail_ptr = ring_buffer.allocation + tail_offset;
    aws_atomic_store_ptr(&ring_buffer.tail, (void *)tail_ptr);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* 2. Set up the byte buf to release */
    struct aws_byte_buf buf;

    /* buf.buffer must point within the ring buffer's allocation */
    size_t buf_offset;
    __CPROVER_assume(buf_offset < ring_size);
    buf.buffer = ring_buffer.allocation + buf_offset;

    /* buf.capacity must be such that buf.buffer + buf.capacity <= allocation_end */
    size_t buf_capacity;
    __CPROVER_assume(buf_capacity > 0);
    __CPROVER_assume(buf_capacity <= (size_t)(ring_buffer.allocation_end - buf.buffer));
    buf.capacity = buf_capacity;

    size_t buf_len;
    __CPROVER_assume(buf_len <= buf_capacity);
    buf.len = buf_len;

    buf.allocator = ring_buffer.allocator;

    /* Ensure buf is valid before calling belongs_to_pool */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Verify buf belongs to the ring buffer */
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    /* 3. Call the function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 4. Assert postconditions */

    /* buf should be zeroed out after release */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);
}
