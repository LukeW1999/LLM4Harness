// === STEP 1: SUCCESS PATH ===
// aws_ring_buffer_release is void, no return value.
// After the call:
//   - ring_buffer->tail (atomic): CHANGES to buf->buffer + buf->capacity
//   - buf: ZEROED (AWS_ZERO_STRUCT(*buf) sets all fields to 0)
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void and always executes.
//
// === STEP 3: FRAME CONDITIONS ===
// ring_buffer (struct aws_ring_buffer):
//   - allocator: UNCHANGED
//   - allocation: UNCHANGED
//   - head: UNCHANGED
//   - tail: CHANGED (set to buf->buffer + buf->capacity)
//   - allocation_end: UNCHANGED
// buf (struct aws_byte_buf):
//   - buffer: CHANGED to NULL (zeroed)
//   - len: CHANGED to 0 (zeroed)
//   - capacity: CHANGED to 0 (zeroed)
//   - allocator: CHANGED to NULL (zeroed)
//
// === STEP 4: VALIDITY INVARIANTS ===
// Before the call, ring_buffer must be valid and buf must belong to the ring buffer.
// After the call:
//   - aws_ring_buffer_is_valid(ring_buffer): The ring buffer should remain valid
//     (tail is updated to a valid pointer within the allocation)
//   - buf is zeroed, so aws_byte_buf_is_valid(buf) may not hold (buffer is NULL, capacity is 0)
//     but that's expected after release.

#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void aws_ring_buffer_release_harness() {
    /* Allocate and initialize the ring buffer */
    struct aws_ring_buffer *ring_buffer = malloc(sizeof(struct aws_ring_buffer));
    __CPROVER_assume(ring_buffer != NULL);

    /* Set up a valid ring buffer with a concrete allocation */
    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(ring_buffer, allocator, ring_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(ring_buffer));

    /* Set up a byte buf that belongs to this ring buffer */
    struct aws_byte_buf buf;
    
    /* buf.buffer must point somewhere within [allocation, allocation_end) */
    /* buf.buffer + buf.capacity must be within [allocation, allocation_end] */
    size_t buf_capacity;
    size_t offset;
    
    /* Choose a valid offset and capacity within the ring buffer allocation */
    size_t alloc_size = (size_t)(ring_buffer->allocation_end - ring_buffer->allocation);
    __CPROVER_assume(offset <= alloc_size);
    __CPROVER_assume(buf_capacity <= alloc_size - offset);
    __CPROVER_assume(buf_capacity > 0);
    
    buf.buffer = ring_buffer->allocation + offset;
    buf.capacity = buf_capacity;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = NULL; /* ring buffer vended buffers don't have allocators */

    /* Verify buf belongs to the ring buffer */
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(ring_buffer, &buf));

    /* Save old state */
    struct aws_ring_buffer old_ring = *ring_buffer;
    uint8_t *expected_new_tail = buf.buffer + buf.capacity;

    /* Call the function under test */
    aws_ring_buffer_release(ring_buffer, &buf);

    /* === FRAME CONDITIONS: ring_buffer fields === */
    /* allocator: UNCHANGED */
    assert(ring_buffer->allocator == old_ring.allocator);
    /* allocation: UNCHANGED */
    assert(ring_buffer->allocation == old_ring.allocation);
    /* allocation_end: UNCHANGED */
    assert(ring_buffer->allocation_end == old_ring.allocation_end);
    /* head: UNCHANGED */
    assert(aws_atomic_load_ptr((struct aws_atomic_var *)&ring_buffer->head) == 
           aws_atomic_load_ptr((struct aws_atomic_var *)&old_ring.head));
    /* tail: CHANGED to buf->buffer + buf->capacity */
    assert(aws_atomic_load_ptr((struct aws_atomic_var *)&ring_buffer->tail) == expected_new_tail);

    /* === FRAME CONDITIONS: buf fields (all zeroed) === */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* === VALIDITY INVARIANTS === */
    assert(aws_ring_buffer_is_valid(ring_buffer));

    /* Clean up */
    aws_ring_buffer_clean_up(ring_buffer);
}
