// === STEP 1: SUCCESS PATH ===
// When aws_ring_buffer_acquire returns AWS_OP_SUCCESS:
//   - ring_buf->head: CHANGES to (allocation + requested_size) or (head_cpy + requested_size)
//   - ring_buf->tail: CHANGES to ring_buf->allocation (only in the head==tail branch)
//   - dest->buffer: CHANGES to point to some location within ring_buf->allocation
//   - dest->len: CHANGES to requested_size
//   - dest->capacity: CHANGES to requested_size
//   - dest->allocator: CHANGES to NULL (aws_byte_buf_from_empty_array sets allocator to NULL)
//
// === STEP 2: FAILURE PATH ===
// When aws_ring_buffer_acquire returns AWS_OP_ERR:
//   - ring_buf->head: UNCHANGED
//   - ring_buf->tail: UNCHANGED
//   - dest: UNCHANGED (not written)
//
// === STEP 3: FRAME CONDITIONS ===
//   ring_buf (struct aws_ring_buffer):
//     - allocator: UNCHANGED always
//     - allocation: UNCHANGED always
//     - head: CHANGED on success, UNCHANGED on failure
//     - tail: CHANGED on success (head==tail branch only), UNCHANGED on failure
//     - allocation_end: UNCHANGED always
//   dest (struct aws_byte_buf):
//     - buffer: CHANGED on success, UNCHANGED on failure
//     - len: CHANGED on success, UNCHANGED on failure
//     - capacity: CHANGED on success, UNCHANGED on failure
//     - allocator: CHANGED on success (set to NULL), UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_ring_buffer_is_valid(ring_buf): YES (must hold after call on success)
//   - aws_byte_buf_is_valid(dest): YES on success

#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

/* We need a bounded buffer size for CBMC to be tractable */
#ifndef MAX_RING_BUFFER_SIZE
#    define MAX_RING_BUFFER_SIZE 16
#endif

void aws_ring_buffer_acquire_harness(void) {
    /* Set up ring buffer */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator;

    /* Choose a bounded size for the ring buffer allocation */
    size_t size;
    __CPROVER_assume(size > 0 && size <= MAX_RING_BUFFER_SIZE);

    /* Initialize the ring buffer */
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Set head and tail to nondeterministic valid values within the allocation */
    uint8_t *head_ptr;
    uint8_t *tail_ptr;

    /* head and tail must be within [allocation, allocation_end] */
    __CPROVER_assume(head_ptr >= ring_buf.allocation && head_ptr <= ring_buf.allocation_end);
    __CPROVER_assume(tail_ptr >= ring_buf.allocation && tail_ptr <= ring_buf.allocation_end);

    /* Store nondeterministic head/tail */
    aws_atomic_store_ptr(&ring_buf.head, (void *)head_ptr);
    aws_atomic_store_ptr(&ring_buf.tail, (void *)tail_ptr);

    /* Verify ring buffer is still valid after setting head/tail */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Set up requested_size - must be nonzero (precondition) */
    size_t requested_size;
    __CPROVER_assume(requested_size > 0 && requested_size <= MAX_RING_BUFFER_SIZE);

    /* Set up dest buffer */
    struct aws_byte_buf dest;
    /* Save old state */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_dest = dest;

    /* Save old head and tail values */
    uint8_t *old_head;
    uint8_t *old_tail;
    AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, old_head);
    AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, old_tail);

    /* Call the function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Frame conditions: allocator and allocation pointers never change */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);

    if (result == AWS_OP_SUCCESS) {
        /* On success: dest must be valid and point within ring buffer */
        assert(dest.len == requested_size);
        assert(dest.capacity == requested_size);
        assert(dest.buffer != NULL);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);
        assert(dest.allocator == NULL);

        /* Ring buffer must still be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));

        /* The buffer must belong to the ring buffer */
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));

        /* Head must have been updated */
        uint8_t *new_head;
        AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
        assert(new_head == dest.buffer + requested_size);

    } else {
        /* On failure: ring buffer state must be unchanged */
        uint8_t *new_head;
        uint8_t *new_tail;
        AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
        AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, new_tail);

        assert(new_head == old_head);
        assert(new_tail == old_tail);

        /* Ring buffer must still be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buf);
}
