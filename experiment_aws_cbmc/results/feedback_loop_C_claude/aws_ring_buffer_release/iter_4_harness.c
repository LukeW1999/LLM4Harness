#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 16

void aws_ring_buffer_release_harness() {
    struct aws_ring_buffer *ring_buffer = malloc(sizeof(struct aws_ring_buffer));
    if (ring_buffer == NULL) return;

    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(ring_buffer, allocator, ring_size);
    if (init_result != AWS_OP_SUCCESS) {
        free(ring_buffer);
        return;
    }

    /* Compute allocation size */
    size_t alloc_size = (size_t)(ring_buffer->allocation_end - ring_buffer->allocation);
    if (alloc_size == 0) {
        aws_ring_buffer_clean_up(ring_buffer);
        free(ring_buffer);
        return;
    }

    /* Choose a nondeterministic offset and capacity within bounds */
    size_t offset;
    size_t buf_capacity;
    __CPROVER_assume(offset < alloc_size);
    __CPROVER_assume(buf_capacity > 0);
    __CPROVER_assume(buf_capacity <= alloc_size - offset);

    uint8_t *buf_start = ring_buffer->allocation + offset;
    uint8_t *buf_end = buf_start + buf_capacity;

    /* buf_end must be within allocation */
    __CPROVER_assume(buf_end >= ring_buffer->allocation);
    __CPROVER_assume(buf_end <= ring_buffer->allocation_end);

    /* Set head == tail == buf_start so ring buffer is valid (empty) */
    aws_atomic_store_ptr(&ring_buffer->head, (void *)buf_start);
    aws_atomic_store_ptr(&ring_buffer->tail, (void *)buf_start);

    /* Verify ring buffer is valid */
    if (!aws_ring_buffer_is_valid(ring_buffer)) {
        aws_ring_buffer_clean_up(ring_buffer);
        free(ring_buffer);
        return;
    }

    /* Set up the byte buf */
    struct aws_byte_buf buf;
    memset(&buf, 0, sizeof(buf));
    buf.buffer = buf_start;
    buf.capacity = buf_capacity;
    buf.len = 0;
    buf.allocator = NULL;

    /* Verify buf belongs to the ring buffer */
    if (!aws_ring_buffer_buf_belongs_to_pool(ring_buffer, &buf)) {
        aws_ring_buffer_clean_up(ring_buffer);
        free(ring_buffer);
        return;
    }

    /* Save state before call */
    struct aws_allocator *old_allocator = ring_buffer->allocator;
    uint8_t *old_allocation = ring_buffer->allocation;
    uint8_t *old_allocation_end = ring_buffer->allocation_end;
    void *old_head = aws_atomic_load_ptr(&ring_buffer->head);
    uint8_t *expected_new_tail = buf_end;

    /* Call the function under test */
    aws_ring_buffer_release(ring_buffer, &buf);

    /* Postconditions on ring_buffer */
    assert(ring_buffer->allocator == old_allocator);
    assert(ring_buffer->allocation == old_allocation);
    assert(ring_buffer->allocation_end == old_allocation_end);
    assert(aws_atomic_load_ptr(&ring_buffer->head) == old_head);
    assert(aws_atomic_load_ptr(&ring_buffer->tail) == (void *)expected_new_tail);

    /* Postconditions on buf (should be zeroed out) */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    aws_ring_buffer_clean_up(ring_buffer);
    free(ring_buffer);
}
