#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_ring_buffer_acquire_harness() {
    /* 1. Set up the ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Allocate the internal buffer with a bounded size */
    size_t buf_size;
    __CPROVER_assume(buf_size > 0 && buf_size <= MAX_BUFFER_SIZE);
    uint8_t *allocation = malloc(buf_size);
    __CPROVER_assume(allocation != NULL);

    ring_buf.allocator = aws_default_allocator();
    ring_buf.allocation = allocation;
    ring_buf.allocation_end = allocation + buf_size;

    /* Set up head and tail atomics to valid positions within the buffer */
    uint8_t *head_ptr;
    uint8_t *tail_ptr;

    /* head and tail must be within [allocation, allocation_end] */
    size_t head_offset = nondet_size_t();
    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(head_offset <= buf_size);
    __CPROVER_assume(tail_offset <= buf_size);

    head_ptr = allocation + head_offset;
    tail_ptr = allocation + tail_offset;

    aws_atomic_store_ptr(&ring_buf.head, head_ptr);
    aws_atomic_store_ptr(&ring_buf.tail, tail_ptr);

    /* Assume the ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Set up the destination byte buffer */
    struct aws_byte_buf dest;
    /* Initialize dest to some state */
    dest.len = 0;
    dest.capacity = 0;
    dest.buffer = NULL;
    dest.allocator = NULL;

    /* 3. Set up requested_size - must be non-zero per precondition */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= buf_size); /* bound for tractability */

    /* 4. Save old state */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *old_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);

    /* 5. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 6. Assert postconditions */

    /* Fields that must never change */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);

    if (result == AWS_OP_SUCCESS) {
        /* On success: dest is populated with a valid buffer of requested_size */
        assert(dest.buffer != NULL);
        assert(dest.len == 0); /* aws_byte_buf_from_empty_array sets len=0 */
        assert(dest.capacity == requested_size);

        /* dest.buffer must be within the ring buffer's allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);

        /* The head must have been updated */
        uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        assert(new_head != NULL);
        assert(new_head >= ring_buf.allocation);
        assert(new_head <= ring_buf.allocation_end);

        /* dest.buffer should equal old_tail when head==tail (empty buffer case),
           or head_cpy in other cases */
        /* The new head should be dest.buffer + requested_size */
        assert(new_head == dest.buffer + requested_size);

        /* Validity must hold after success */
        assert(aws_ring_buffer_is_valid(&ring_buf));

        /* dest buffer must be valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* The buffer belongs to this ring buffer */
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));

    } else {
        /* On failure: ring buffer state should be unchanged */
        /* tail should be unchanged on failure */
        uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
        assert(new_tail == old_tail);

        /* Ring buffer validity must still hold */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }
}
