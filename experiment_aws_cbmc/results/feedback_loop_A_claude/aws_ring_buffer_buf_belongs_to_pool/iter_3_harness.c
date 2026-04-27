#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* 1. Set up ring_buffer with a concrete backing allocation */
    struct aws_ring_buffer ring_buffer;

    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);
    uint8_t *allocation = (uint8_t *)malloc(ring_size);
    __CPROVER_assume(allocation != NULL);

    ring_buffer.allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    ring_buffer.allocation = allocation;
    ring_buffer.allocation_end = allocation + ring_size;

    /* Set head and tail atomics to valid pointers within the allocation */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= ring_size);
    __CPROVER_assume(tail_offset <= ring_size);
    aws_atomic_init_ptr(&ring_buffer.head, (void *)(allocation + head_offset));
    aws_atomic_init_ptr(&ring_buffer.tail, (void *)(allocation + tail_offset));

    /* 2. Set up byte_buf with a fully nondeterministic buffer pointer */
    struct aws_byte_buf buf;
    buf.allocator = NULL;
    buf.len = 0;
    buf.capacity = 0;

    /* Nondeterministic pointer - can be anything */
    uint8_t *buf_ptr;
    buf.buffer = buf_ptr;

    /* 3. Save old state before calling function */
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;
    uint8_t *old_buf_buffer = buf.buffer;

    /* 4. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* 5. Assert postconditions */

    /* The result should be true iff buf.buffer is in [allocation, allocation_end) */
    bool expected = (buf.buffer != NULL) &&
                    ((uintptr_t)buf.buffer >= (uintptr_t)ring_buffer.allocation) &&
                    ((uintptr_t)buf.buffer < (uintptr_t)ring_buffer.allocation_end);

    assert(result == expected);

    /* If result is true, buf->buffer must be within the ring buffer's allocation */
    if (result) {
        assert(buf.buffer != NULL);
        assert((uintptr_t)buf.buffer >= (uintptr_t)ring_buffer.allocation);
        assert((uintptr_t)buf.buffer < (uintptr_t)ring_buffer.allocation_end);
    }

    /* If buf->buffer is within range, result must be true */
    if (buf.buffer != NULL &&
        (uintptr_t)buf.buffer >= (uintptr_t)ring_buffer.allocation &&
        (uintptr_t)buf.buffer < (uintptr_t)ring_buffer.allocation_end) {
        assert(result);
    }

    /* If buf->buffer is outside range, result must be false */
    if (buf.buffer == NULL ||
        (uintptr_t)buf.buffer < (uintptr_t)ring_buffer.allocation ||
        (uintptr_t)buf.buffer >= (uintptr_t)ring_buffer.allocation_end) {
        assert(!result);
    }

    /* The function is a pure query - nothing should change */
    assert(ring_buffer.allocation == old_allocation);
    assert(ring_buffer.allocation_end == old_allocation_end);
    assert(buf.buffer == old_buf_buffer);
}
