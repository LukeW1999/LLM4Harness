#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 16

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring_buffer;

    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, ring_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    struct aws_byte_buf buf;

    /* Use the simplest case: buf starts at allocation start */
    buf.buffer = ring_buffer.allocation;

    size_t buf_capacity;
    __CPROVER_assume(buf_capacity > 0 && buf_capacity <= ring_size);
    buf.capacity = buf_capacity;

    __CPROVER_assume(buf.buffer + buf.capacity <= ring_buffer.allocation_end);

    buf.len = 0;
    buf.allocator = allocator;

    /* Set head to buf.buffer, tail to buf.buffer + buf.capacity */
    atomic_store(&ring_buffer.head, (uintptr_t)buf.buffer);
    atomic_store(&ring_buffer.tail, (uintptr_t)(buf.buffer + buf.capacity));

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    struct aws_allocator *old_allocator = ring_buffer.allocator;
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;

    aws_ring_buffer_release(&ring_buffer, &buf);

    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    assert(ring_buffer.allocator == old_allocator);
    assert(ring_buffer.allocation == old_allocation);
    assert(ring_buffer.allocation_end == old_allocation_end);

    aws_ring_buffer_clean_up(&ring_buffer);
}
