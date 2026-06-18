#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256

void aws_ring_buffer_acquire_harness(void) {
    /* 1. Allocate and initialise the ring buffer */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;

    /* Allocate underlying storage for the ring buffer */
    struct aws_byte_buf pool_buf;
    ensure_byte_buf_has_allocated_buffer_member(&pool_buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&pool_buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(pool_buf.capacity > 0);

    ring_buf.allocation = pool_buf.buffer;
    ring_buf.allocation_end = pool_buf.buffer + pool_buf.capacity;

    /* Nondeterministically choose head and tail pointers within the allocation */
    uint8_t *head_ptr;
    uint8_t *tail_ptr;
    __CPROVER_assume(head_ptr >= ring_buf.allocation && head_ptr <= ring_buf.allocation_end);
    __CPROVER_assume(tail_ptr >= ring_buf.allocation && tail_ptr <= ring_buf.allocation_end);
    AWS_ATOMIC_STORE_HEAD_PTR(&ring_buf, head_ptr);
    AWS_ATOMIC_STORE_TAIL_PTR(&ring_buf, tail_ptr);

    /* Assume the ring buffer is valid with the chosen pointers */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Allocate destination byte buffer */
    struct aws_byte_buf dest;
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    /* 3. Save old state */
    struct aws_ring_buffer old_ring = ring_buf;
    struct aws_byte_buf old_dest = dest;

    /* 4. Nondet requested size, bounded and non‑zero */
    size_t requested_size;
    __CPROVER_assume(requested_size > 0 && requested_size <= MAX_BUFFER_SIZE);

    /* 5. Call the function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest must describe a buffer of exactly the requested size */
        assert(dest.len == requested_size);
        assert(dest.capacity == requested_size);
        /* the buffer must lie within the ring buffer's allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);
        /* the buffer must belong to the pool */
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));

        /* head must have advanced by requested_size (modulo wrap) */
        uint8_t *old_head;
        AWS_ATOMIC_LOAD_HEAD_PTR(&old_ring, old_head);
        uint8_t *new_head;
        AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
        bool head_advanced = (new_head == old_head + requested_size) ||
                             (new_head == ring_buf.allocation && old_head + requested_size > ring_buf.allocation_end);
        assert(head_advanced);
    } else {
        /* On failure the ring buffer must be unchanged */
        assert(ring_buf.allocator == old_ring.allocator);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
        uint8_t *old_head;
        uint8_t *old_tail;
        AWS_ATOMIC_LOAD_HEAD_PTR(&old_ring, old_head);
        AWS_ATOMIC_LOAD_TAIL_PTR(&old_ring, old_tail);
        uint8_t *new_head;
        uint8_t *new_tail;
        AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
        AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, new_tail);
        assert(new_head == old_head);
        assert(new_tail == old_tail);

        /* dest must be unchanged */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
    }

    /* 7. Fields that never change regardless of result */
    assert(ring_buf.allocator == old_ring.allocator);
    assert(ring_buf.allocation == old_ring.allocation);
    assert(ring_buf.allocation_end == old_ring.allocation_end);

    /* 8. Ring buffer validity invariant must hold */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
