#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Maximum allocation size for the ring buffer in the proof */
#define MAX_RING_BUFFER_SIZE 16

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf dest;

    size_t ring_size;
    __CPROVER_assume(ring_size >= 2 && ring_size <= MAX_RING_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, ring_size);
    if (init_result != AWS_OP_SUCCESS) {
        return;
    }

    /* Set up nondet head and tail pointers within valid bounds */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset < ring_size);
    __CPROVER_assume(tail_offset < ring_size);

    uint8_t *head_ptr = ring_buf.allocation + head_offset;
    uint8_t *tail_ptr = ring_buf.allocation + tail_offset;

    aws_atomic_store_ptr(&ring_buf.head, (void *)head_ptr);
    aws_atomic_store_ptr(&ring_buf.tail, (void *)tail_ptr);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    size_t requested_size;
    __CPROVER_assume(requested_size > 0 && requested_size <= MAX_RING_BUFFER_SIZE);

    struct aws_allocator *pre_allocator = ring_buf.allocator;
    uint8_t *pre_allocation = ring_buf.allocation;
    uint8_t *pre_allocation_end = ring_buf.allocation_end;

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Postcondition: result is success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.buffer != NULL);
        assert(dest.len == requested_size);
        assert(dest.capacity == requested_size);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
    }

    /* Frame conditions */
    assert(ring_buf.allocator == pre_allocator);
    assert(ring_buf.allocation == pre_allocation);
    assert(ring_buf.allocation_end == pre_allocation_end);

    assert(aws_ring_buffer_is_valid(&ring_buf));

    aws_ring_buffer_clean_up(&ring_buf);
}
