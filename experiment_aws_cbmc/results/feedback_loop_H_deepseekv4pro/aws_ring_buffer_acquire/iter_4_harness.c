#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

#define MAX_RING_BUFFER_ALLOCATION 10
#define MAX_BUFFER_SIZE 256

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf dest;
    size_t requested_size;

    ring_buf.allocator = aws_default_allocator();
    size_t alloc_size;
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_RING_BUFFER_ALLOCATION);
    ring_buf.allocation = (uint8_t *)malloc(alloc_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + alloc_size;

    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    __CPROVER_assume(requested_size <= alloc_size);

    uint8_t *old_head, *old_tail;
    AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, &old_head);
    AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, &old_tail);
    struct aws_byte_buf old_dest = dest;

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    uint8_t *new_head, *new_tail;
    AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, &new_head);
    AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, &new_tail);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.buffer != NULL);
        assert(dest.len == requested_size);
        assert(dest.capacity == requested_size);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        assert(old_head == new_head);
        assert(old_tail == new_tail);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }

    assert(ring_buf.allocator == aws_default_allocator());
    assert(ring_buf.allocation_end == ring_buf.allocation + alloc_size);

    free(ring_buf.allocation);
    free(old_dest.buffer);
}
