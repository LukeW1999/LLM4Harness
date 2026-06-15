/* CBMC harness for aws_ring_buffer_release */

#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void aws_ring_buffer_release_harness(void) {
    /* Allocate and initialize ring buffer */
    struct aws_ring_buffer ring_buffer;
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size < 4096);

    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, ring_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Save immutable fields for frame condition checks */
    struct aws_allocator *orig_allocator = ring_buffer.allocator;
    uint8_t *orig_allocation = ring_buffer.allocation;
    uint8_t *orig_allocation_end = ring_buffer.allocation_end;

    /* Prepare a valid empty destination byte buffer */
    struct aws_byte_buf buf;
    buf.buffer = NULL;
    buf.capacity = 0;
    buf.len = 0;
    buf.allocator = NULL;
    __CPROVER_assume(buf.buffer == NULL);
    __CPROVER_assume(buf.capacity == 0);
    __CPROVER_assume(buf.len == 0);
    __CPROVER_assume(buf.allocator == NULL);

    /* Acquire a buffer from the ring buffer */
    size_t capacity;
    __CPROVER_assume(capacity > 0 && capacity <= ring_size);

    int acquire_result = aws_ring_buffer_acquire(&ring_buffer, &buf, capacity);
    __CPROVER_assume(acquire_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    /* Release the acquired buffer */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Post‑condition: buf is zeroed */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* Frame conditions: ring buffer immutable fields unchanged */
    assert(ring_buffer.allocator == orig_allocator);
    assert(ring_buffer.allocation == orig_allocation);
    assert(ring_buffer.allocation_end == orig_allocation_end);

    /* Ring buffer must remain valid after release */
    assert(aws_ring_buffer_is_valid(&ring_buffer));

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buffer);
}
