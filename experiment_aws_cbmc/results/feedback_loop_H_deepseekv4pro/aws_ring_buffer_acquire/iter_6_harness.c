#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_RING_BUFFER_ALLOCATION 10
#define MAX_BUFFER_SIZE 256

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf dest;
    size_t requested_size;

    /* Allocate the ring buffer's backing memory */
    ring_buf.allocator = aws_default_allocator();
    size_t alloc_size;
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_RING_BUFFER_ALLOCATION);
    ring_buf.allocation = (uint8_t *)malloc(alloc_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + alloc_size;

    /* Set head and tail to arbitrary valid positions within the allocation */
    uint8_t *head_ptr, *tail_ptr;
    __CPROVER_assume(head_ptr >= ring_buf.allocation && head_ptr < ring_buf.allocation_end);
    __CPROVER_assume(tail_ptr >= ring_buf.allocation && tail_ptr < ring_buf.allocation_end);
    aws_atomic_store_ptr(&ring_buf.head, head_ptr);
    aws_atomic_store_ptr(&ring_buf.tail, tail_ptr);

    /* Prepare a properly allocated byte_buf (its buffer will be overwritten on success) */
    ensure_byte_buf_has_allocated_buffer_member(&dest, MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* Save state before the call */
    uint8_t *old_head, *old_tail;
    aws_atomic_load_ptr(&ring_buf.head, (void **)&old_head);
    aws_atomic_load_ptr(&ring_buf.tail, (void **)&old_tail);
    struct aws_byte_buf old_dest = dest;

    /* Requested size: positive, bounded, may exceed available space to exercise error path */
    __CPROVER_assume(requested_size > 0 && requested_size <= MAX_BUFFER_SIZE);

    /* The ring buffer must be valid according to its own contract */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Call the function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Read back the possibly modified atomic pointers */
    uint8_t *new_head, *new_tail;
    aws_atomic_load_ptr(&ring_buf.head, (void **)&new_head);
    aws_atomic_load_ptr(&ring_buf.tail, (void **)&new_tail);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.buffer != NULL);
        assert(dest.capacity == requested_size);
        assert(dest.len == 0);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
        assert(new_head != old_head);          /* head moved forward */
        assert(new_tail == old_tail);         /* tail unchanged */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* On failure nothing should have changed */
        assert(old_head == new_head);
        assert(old_tail == new_tail);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }

    /* Invariants that hold in both paths */
    assert(ring_buf.allocator == aws_default_allocator());
    assert(ring_buf.allocation_end == ring_buf.allocation + alloc_size);

    free(ring_buf.allocation);
    free(old_dest.buffer);
}
