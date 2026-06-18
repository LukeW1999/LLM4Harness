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

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    size_t ring_size = nondet_size_t();
    size_t requested_size = nondet_size_t();

    __CPROVER_assume(ring_size > 1);
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size < ring_size);

    int init_result = aws_ring_buffer_init(&ring_buffer, aws_default_allocator(), ring_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    int acquire_result = aws_ring_buffer_acquire(&ring_buffer, requested_size, &buf);
    __CPROVER_assume(acquire_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    struct aws_byte_buf old_buf = buf;
    struct aws_allocator *old_ring_allocator = ring_buffer.allocator;
    uint8_t *old_ring_allocation = ring_buffer.allocation;
    uint8_t *old_ring_allocation_end = ring_buffer.allocation_end;
    uint8_t *old_ring_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);

    aws_ring_buffer_release(&ring_buffer, &buf);

    assert((uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail) == old_buf.buffer + old_buf.capacity);

    assert(buf.allocator == NULL);
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    assert(ring_buffer.allocator == old_ring_allocator);
    assert(ring_buffer.allocation == old_ring_allocation);
    assert((uint8_t *)aws_atomic_load_ptr(&ring_buffer.head) == old_ring_head);
    assert(ring_buffer.allocation_end == old_ring_allocation_end);

    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));
}
