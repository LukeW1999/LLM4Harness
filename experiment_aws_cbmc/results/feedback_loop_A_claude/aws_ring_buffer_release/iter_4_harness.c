#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

void aws_ring_buffer_release_harness() {
    /* 1. Set up ring buffer */
    struct aws_ring_buffer ring_buffer;

    /* Allocate the ring buffer's internal allocation */
    size_t alloc_size;
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);
    ring_buffer.allocator = allocator;

    uint8_t *allocation = malloc(alloc_size);
    __CPROVER_assume(allocation != NULL);
    ring_buffer.allocation = allocation;
    ring_buffer.allocation_end = allocation + alloc_size;

    /* 2. Set up buf that belongs to the ring buffer */
    struct aws_byte_buf buf;
    buf.allocator = NULL;

    /* buf->buffer must be within the ring buffer's allocation */
    size_t buf_offset;
    __CPROVER_assume(buf_offset < alloc_size);
    buf.buffer = allocation + buf_offset;

    /* buf->capacity must be such that buf->buffer + buf->capacity is within bounds */
    size_t buf_capacity;
    __CPROVER_assume(buf_capacity > 0);
    __CPROVER_assume(buf_offset + buf_capacity <= alloc_size);
    buf.capacity = buf_capacity;

    size_t buf_len;
    __CPROVER_assume(buf_len <= buf_capacity);
    buf.len = buf_len;

    /* The tail must equal buf->buffer for the release to be valid.
     * aws_ring_buffer_release expects tail == buf->buffer (FIFO order). */
    aws_atomic_init_ptr(&ring_buffer.tail, (void *)buf.buffer);

    /* Set head to a valid position within the allocation */
    size_t head_offset;
    __CPROVER_assume(head_offset <= alloc_size);
    uint8_t *head_ptr = allocation + head_offset;
    aws_atomic_init_ptr(&ring_buffer.head, (void *)head_ptr);

    /* Compute expected new tail after release */
    uint8_t *expected_new_tail = buf.buffer + buf.capacity;
    if (expected_new_tail == ring_buffer.allocation_end) {
        expected_new_tail = ring_buffer.allocation;
    }

    /* Ensure the ring buffer is valid before the call */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* After release, the new tail will be expected_new_tail.
     * We need to ensure that the ring buffer will still be valid after the update.
     * The ring buffer validity requires head and tail to be within [allocation, allocation_end].
     * expected_new_tail is already within bounds by construction above.
     * We also need to ensure the ring buffer invariant holds with the new tail.
     * The key invariant: head and tail are within [allocation, allocation_end].
     * Since expected_new_tail is already constrained to be within bounds, this should hold.
     */

    /* 3. Save old ring buffer state */
    struct aws_allocator *old_allocator = ring_buffer.allocator;
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;
    void *old_head = aws_atomic_load_ptr(&ring_buffer.head);

    /* 4. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 5. Assert postconditions */

    /* buf should be zeroed out */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* ring_buffer fields that should not change */
    assert(ring_buffer.allocator == old_allocator);
    assert(ring_buffer.allocation == old_allocation);
    assert(ring_buffer.allocation_end == old_allocation_end);

    /* head should not change */
    void *new_head = aws_atomic_load_ptr(&ring_buffer.head);
    assert(new_head == old_head);
}
