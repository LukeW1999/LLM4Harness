#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_ALLOC_SIZE 1024

void aws_ring_buffer_release_harness() {
    /* Allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* Allocate memory for the ring buffer */
    size_t alloc_size = nondet_uint();
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_ALLOC_SIZE);
    uint8_t *allocation = (uint8_t *)malloc(alloc_size);
    __CPROVER_assume(allocation != NULL);

    /* Initialise ring buffer */
    struct aws_ring_buffer ring_buffer;
    ring_buffer.allocator = alloc;
    ring_buffer.allocation = allocation;
    ring_buffer.allocation_end = allocation + alloc_size;
    ring_buffer.head = allocation;
    aws_atomic_store_ptr(&ring_buffer.tail, allocation);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Set up a byte buffer that belongs to the ring buffer */
    struct aws_byte_buf buf;
    size_t buf_size = nondet_uint();
    __CPROVER_assume(buf_size > 0 && buf_size <= (size_t)(ring_buffer.allocation_end - ring_buffer.head));
    buf.buffer = ring_buffer.head;
    buf.capacity = buf_size;
    buf.len = buf_size;
    buf.allocator = alloc;

    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    /* Save old state */
    struct aws_ring_buffer old_ring = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Post‑conditions */
    assert(aws_ring_buffer_check_atomic_ptr(&ring_buffer,
                                            old_buf.buffer + old_buf.capacity));

    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    assert(ring_buffer.allocator == old_ring.allocator);
    assert(ring_buffer.allocation == old_ring.allocation);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);
    assert(ring_buffer.head == old_ring.head);

    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
