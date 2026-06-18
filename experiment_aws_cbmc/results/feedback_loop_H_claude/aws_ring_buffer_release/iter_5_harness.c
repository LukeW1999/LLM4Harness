#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_BUFFER_SIZE
#    define MAX_BUFFER_SIZE 64
#endif

void aws_ring_buffer_release_harness() {
    /* 1. Set up ring buffer */
    struct aws_ring_buffer ring_buffer;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Use a bounded size for the ring buffer */
    size_t size;
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    /* Initialize the ring buffer */
    if (aws_ring_buffer_init(&ring_buffer, allocator, size) != AWS_OP_SUCCESS) {
        return;
    }
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* 2. Set up a byte_buf that belongs to this ring buffer */
    struct aws_byte_buf buf;

    /* Set buf.buffer to a valid location within the ring buffer allocation */
    buf.buffer = ring_buffer.allocation;

    /* buf.capacity must be such that buf.buffer + buf.capacity is strictly within allocation_end */
    size_t max_capacity = (size_t)(ring_buffer.allocation_end - ring_buffer.allocation);
    __CPROVER_assume(buf.capacity > 0 && buf.capacity < max_capacity);
    __CPROVER_assume(buf.buffer + buf.capacity < ring_buffer.allocation_end);

    buf.len = 0;
    buf.allocator = NULL;

    /* Verify buf belongs to the ring buffer */
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    /* 3. Save old state */
    uint8_t *expected_new_tail = buf.buffer + buf.capacity;
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    struct aws_allocator *old_allocator = ring_buffer.allocator;
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;

    /* 4. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 5. Assert postconditions */

    /* The buf should be zeroed out after release (AWS_ZERO_STRUCT(*buf)) */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* Ring buffer fields that should not change */
    assert(ring_buffer.allocator == old_allocator);
    assert(ring_buffer.allocation == old_allocation);
    assert(ring_buffer.allocation_end == old_allocation_end);

    /* Head should not change */
    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    assert(new_head == old_head);

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buffer);
}
