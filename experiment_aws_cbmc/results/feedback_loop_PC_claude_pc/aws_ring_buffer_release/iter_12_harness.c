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
    __CPROVER_assume(ring_size > 1 && ring_size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, ring_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    size_t acquire_size;
    __CPROVER_assume(acquire_size > 0 && acquire_size < ring_size);

    struct aws_byte_buf buf;
    /* Initialize buf to a valid state before passing to acquire */
    buf.buffer = NULL;
    buf.len = 0;
    buf.capacity = 0;
    buf.allocator = NULL;

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    int acquire_result = aws_ring_buffer_acquire(&ring_buffer, acquire_size, &buf);
    __CPROVER_assume(acquire_result == AWS_OP_SUCCESS);
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
