#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_acquire_harness(void) {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring_buf;
    ring_buf.allocator = aws_default_allocator();

    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_BUFFER_SIZE);
    ring_buf.allocation = malloc(alloc_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + alloc_size;

    /* nondet head and tail pointers within the allocation */
    uint8_t *head_ptr = ring_buf.allocation + (nondet_size_t() % alloc_size);
    uint8_t *tail_ptr = ring_buf.allocation + (nondet_size_t() % alloc_size);
    __CPROVER_assume(head_ptr >= ring_buf.allocation && head_ptr <= ring_buf.allocation_end);
    __CPROVER_assume(tail_ptr >= ring_buf.allocation && tail_ptr <= ring_buf.allocation_end);
    AWS_ATOMIC_STORE_HEAD_PTR(&ring_buf, head_ptr);
    AWS_ATOMIC_STORE_TAIL_PTR(&ring_buf, tail_ptr);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Declare and bound the destination byte buffer */
    struct aws_byte_buf dest;
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    /* 3. Save old state */
    struct aws_ring_buffer old_ring = ring_buf;
    struct aws_byte_buf old_dest = dest;

    /* 4. Nondet requested size, bounded and non‑zero */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size > 0 && requested_size <= MAX_BUFFER_SIZE);

    /* 5. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest must describe a buffer of the requested size inside the ring allocation */
        assert(dest.len == requested_size);
        assert(dest.capacity == requested_size);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);

        /* head must have moved forward */
        uint8_t *new_head;
        uint8_t *old_head;
        AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
        AWS_ATOMIC_LOAD_HEAD_PTR(&old_ring, old_head);
        assert(new_head != old_head);

        /* tail may change only when the buffer was previously empty */
        uint8_t *new_tail;
        uint8_t *old_tail;
        AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, new_tail);
        AWS_ATOMIC_LOAD_TAIL_PTR(&old_ring, old_tail);
        if (old_head == old_tail) {
            /* empty case: tail is set to the start of the allocation */
            assert(new_tail == ring_buf.allocation);
        } else {
            /* non‑empty case: tail must stay the same */
            assert(new_tail == old_tail);
        }
    } else {
        /* On failure the ring buffer must be unchanged */
        uint8_t *new_head;
        uint8_t *old_head;
        AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
        AWS_ATOMIC_LOAD_HEAD_PTR(&old_ring, old_head);
        assert(new_head == old_head);

        uint8_t *new_tail;
        uint8_t *old_tail;
        AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, new_tail);
        AWS_ATOMIC_LOAD_TAIL_PTR(&old_ring, old_tail);
        assert(new_tail == old_tail);

        /* dest must be unchanged */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
    }

    /* 7. Fields that never change */
    assert(ring_buf.allocator == old_ring.allocator);
    assert(ring_buf.allocation == old_ring.allocation);
    assert(ring_buf.allocation_end == old_ring.allocation_end);

    /* 8. Validity invariant */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
