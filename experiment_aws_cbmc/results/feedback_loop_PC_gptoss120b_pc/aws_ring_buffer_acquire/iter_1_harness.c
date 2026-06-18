#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_acquire_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf dest;

    /* allocator */
    ring_buf.allocator = aws_default_allocator();

    /* nondet allocation size, bounded */
    size_t alloc_size;
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(alloc_size > 0);

    /* allocate the underlying buffer */
    ring_buf.allocation = (uint8_t *)malloc(alloc_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + alloc_size;

    /* initialize atomic head and tail to a valid state (may be empty or non‑empty) */
    /* nondet decide whether buffer is empty */
    bool empty = nondet_bool();
    if (empty) {
        AWS_ATOMIC_STORE_HEAD_PTR(&ring_buf, ring_buf.allocation);
        AWS_ATOMIC_STORE_TAIL_PTR(&ring_buf, ring_buf.allocation);
    } else {
        /* pick a nondet head and tail inside the allocation, ensuring validity */
        uint8_t *head_ptr = ring_buf.allocation + (nondet_size_t() % alloc_size);
        uint8_t *tail_ptr = ring_buf.allocation + (nondet_size_t() % alloc_size);
        /* ensure they are not equal to avoid the empty case */
        __CPROVER_assume(head_ptr != tail_ptr);
        AWS_ATOMIC_STORE_HEAD_PTR(&ring_buf, head_ptr);
        AWS_ATOMIC_STORE_TAIL_PTR(&ring_buf, tail_ptr);
    }

    /* ensure dest has an allocated buffer member for snapshotting */
    ensure_byte_buf_has_allocated_buffer_member(&dest);

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring_buf;
    struct aws_byte_buf old_dest = dest;

    /* precondition from Doxygen: requested_size != 0 */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    /* assume the ring buffer is valid before the call */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 3. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Load atomic pointers after the call */
    uint8_t *new_head;
    uint8_t *new_tail;
    AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
    AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, new_tail);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* dest must describe a buffer inside the ring allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
        assert(dest.len == requested_size);
        assert(dest.capacity == requested_size);

        /* head must have advanced by exactly requested_size (modulo wrap) */
        /* compute expected new head based on old state */
        if (old_ring.head.value == old_ring.tail.value) {
            /* empty case: head set to allocation + requested_size */
            assert(new_head == ring_buf.allocation + requested_size);
        } else {
            /* non‑empty case: head advanced forward by requested_size */
            uint8_t *expected = (uint8_t *)old_ring.head.value + requested_size;
            /* wrap‑around cannot happen in this function (it only moves forward within bounds) */
            assert(new_head == expected);
        }

        /* tail must be unchanged */
        assert(new_tail == (uint8_t *)old_ring.tail.value);
    } else {
        /* on failure, ring buffer must be unchanged */
        assert(new_head == (uint8_t *)old_ring.head.value);
        assert(new_tail == (uint8_t *)old_ring.tail.value);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
        assert(ring_buf.allocator == old_ring.allocator);

        /* dest must be unchanged */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(ring_buf.allocation == old_ring.allocation);
    assert(ring_buf.allocation_end == old_ring.allocation_end);
    assert(ring_buf.allocator == old_ring.allocator);

    /* 6. Assert validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
