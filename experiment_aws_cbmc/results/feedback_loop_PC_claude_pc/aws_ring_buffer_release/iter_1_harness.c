#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

/*
 * Harness for aws_ring_buffer_release.
 *
 * From the implementation:
 *   AWS_ATOMIC_STORE_TAIL_PTR(ring_buffer, buf->buffer + buf->capacity);
 *   AWS_ZERO_STRUCT(*buf);
 *
 * So:
 *   1. ring_buffer->tail is updated to buf->buffer + buf->capacity
 *   2. *buf is zeroed out (all fields become 0/NULL)
 *   3. ring_buffer->allocator, ->allocation, ->head, ->allocation_end are unchanged
 *   4. ring_buffer remains valid after the call
 */

void aws_ring_buffer_release_harness(void) {
    /* 1. Set up the ring buffer */
    struct aws_ring_buffer ring_buffer;

    /* We need a valid ring buffer. Use aws_ring_buffer_init to create one. */
    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);

    int init_result = aws_ring_buffer_init(&ring_buffer, aws_default_allocator(), ring_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* 2. Set up the byte buf that "belongs" to this ring buffer.
     *    buf->buffer must point somewhere within [allocation, allocation_end)
     *    and buf->buffer + buf->capacity must be within [allocation, allocation_end].
     */
    struct aws_byte_buf buf;

    /* buf.buffer must be within the ring buffer's allocation */
    size_t offset;
    __CPROVER_assume(offset < ring_size);
    buf.buffer = ring_buffer.allocation + offset;

    /* buf.capacity: buf.buffer + buf.capacity must be <= allocation_end */
    size_t remaining = (size_t)(ring_buffer.allocation_end - buf.buffer);
    __CPROVER_assume(buf.capacity <= remaining);
    __CPROVER_assume(buf.capacity > 0);

    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = aws_default_allocator();

    /* Verify buf belongs to the ring buffer */
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    /* 3. Save old state of ring_buffer fields that should NOT change */
    struct aws_allocator *old_allocator = ring_buffer.allocator;
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;

    /* Save the expected new tail value */
    uint8_t *expected_new_tail = buf.buffer + buf.capacity;

    /* 4. Call the function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 5. Assert postconditions */

    /* buf must be zeroed out */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* ring_buffer fields that must not change */
    assert(ring_buffer.allocator == old_allocator);
    assert(ring_buffer.allocation == old_allocation);
    assert(ring_buffer.allocation_end == old_allocation_end);

    /* ring_buffer->tail should now point to buf->buffer + buf->capacity */
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    assert(new_tail == expected_new_tail);

    /* ring_buffer remains valid */
    assert(aws_ring_buffer_is_valid(&ring_buffer));

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buffer);
}

void aws_ring_buffer_release_harness(void) {
    aws_ring_buffer_release_harness();
    return 0;
}
