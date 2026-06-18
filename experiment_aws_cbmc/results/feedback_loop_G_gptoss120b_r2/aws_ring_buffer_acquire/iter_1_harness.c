#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_acquire_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf dest;

    /* allocate ring buffer memory */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);
    ring_buf.allocation = alloc_size ? malloc(alloc_size) : NULL;
    ring_buf.allocation_end = ring_buf.allocation ? ring_buf.allocation + alloc_size : NULL;
    ring_buf.allocator = aws_default_allocator();

    /* nondet initialize head and tail pointers within allocation bounds */
    uint8_t *head_ptr = ring_buf.allocation;
    uint8_t *tail_ptr = ring_buf.allocation;
    if (ring_buf.allocation) {
        size_t head_offset = nondet_size_t() % (alloc_size + 1);
        size_t tail_offset = nondet_size_t() % (alloc_size + 1);
        head_ptr = ring_buf.allocation + head_offset;
        tail_ptr = ring_buf.allocation + tail_offset;
    }
    AWS_ATOMIC_STORE_HEAD_PTR(&ring_buf, head_ptr);
    AWS_ATOMIC_STORE_TAIL_PTR(&ring_buf, tail_ptr);

    /* assume ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring_buf;
    uint8_t *old_head = NULL;
    uint8_t *old_tail = NULL;
    AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, old_head);
    AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, old_tail);
    struct aws_byte_buf old_dest = dest;

    /* 3. Nondet request size with precondition */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    /* 4. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest must point inside the ring buffer allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
        /* capacity should match the requested size */
        assert(dest.capacity == requested_size);
        /* length is expected to be zero for an empty buffer */
        assert(dest.len == 0);
        /* the buffer must belong to this ring buffer's pool */
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
    } else {
        /* on failure the ring buffer state must be unchanged */
        uint8_t *new_head = NULL;
        uint8_t *new_tail = NULL;
        AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
        AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, new_tail);
        assert(new_head == old_head);
        assert(new_tail == old_tail);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
        assert(ring_buf.allocator == old_ring.allocator);
        /* dest must be unchanged */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.len == old_dest.len);
    }

    /* 6. Fields that must never change */
    assert(ring_buf.allocator == old_ring.allocator);
    assert(ring_buf.allocation == old_ring.allocation);
    assert(ring_buf.allocation_end == old_ring.allocation_end);

    /* 7. Validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
