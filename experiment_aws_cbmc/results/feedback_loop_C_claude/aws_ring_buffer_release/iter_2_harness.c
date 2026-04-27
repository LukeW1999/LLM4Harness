#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 64

void aws_ring_buffer_release_harness() {
    /* Allocate and initialize the ring buffer */
    struct aws_ring_buffer *ring_buffer = malloc(sizeof(struct aws_ring_buffer));
    if (ring_buffer == NULL) return;

    /* Set up a valid ring buffer with a concrete allocation */
    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(ring_buffer, allocator, ring_size);
    if (init_result != AWS_OP_SUCCESS) return;
    if (!aws_ring_buffer_is_valid(ring_buffer)) return;

    /* Set up a byte buf that belongs to this ring buffer */
    struct aws_byte_buf buf;
    memset(&buf, 0, sizeof(buf));

    /* buf.buffer must point somewhere within [allocation, allocation_end) */
    size_t alloc_size = (size_t)(ring_buffer->allocation_end - ring_buffer->allocation);
    if (alloc_size == 0) {
        aws_ring_buffer_clean_up(ring_buffer);
        return;
    }

    size_t offset;
    size_t buf_capacity;

    __CPROVER_assume(offset < alloc_size);
    __CPROVER_assume(buf_capacity > 0);
    __CPROVER_assume(buf_capacity <= alloc_size - offset);

    buf.buffer = ring_buffer->allocation + offset;
    buf.capacity = buf_capacity;
    buf.len = 0;
    buf.allocator = NULL;

    /* Verify buf belongs to the ring buffer */
    if (!aws_ring_buffer_buf_belongs_to_pool(ring_buffer, &buf)) {
        aws_ring_buffer_clean_up(ring_buffer);
        return;
    }

    /* Save old state */
    struct aws_allocator *old_allocator = ring_buffer->allocator;
    uint8_t *old_allocation = ring_buffer->allocation;
    uint8_t *old_allocation_end = ring_buffer->allocation_end;
    void *old_head = aws_atomic_load_ptr(&ring_buffer->head);
    uint8_t *expected_new_tail = buf.buffer + buf.capacity;

    /* Call the function under test */
    aws_ring_buffer_release(ring_buffer, &buf);

    /* === POSTCONDITIONS: ring_buffer fields === */
    assert(ring_buffer->allocator == old_allocator);
    assert(ring_buffer->allocation == old_allocation);
    assert(ring_buffer->allocation_end == old_allocation_end);
    assert(aws_atomic_load_ptr(&ring_buffer->head) == old_head);
    assert(aws_atomic_load_ptr(&ring_buffer->tail) == (void *)expected_new_tail);

    /* === POSTCONDITIONS: buf fields (all zeroed) === */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* === VALIDITY INVARIANTS === */
    assert(aws_ring_buffer_is_valid(ring_buffer));

    /* Clean up */
    aws_ring_buffer_clean_up(ring_buffer);
}
