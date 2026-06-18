#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

void aws_ring_buffer_acquire_harness(void) {
    /* 1. Declare data structures */
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf dest;

    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;

    /* nondet allocation size, bounded */
    size_t alloc_size;
    __CPROVER_assume(alloc_size > 0);
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);

    /* allocate the underlying buffer */
    ring_buf.allocation = (uint8_t *)malloc(alloc_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + alloc_size;

    /* initialize atomic head and tail to a valid state (may be empty or non‑empty) */
    bool empty = nondet_bool();
    if (empty) {
        AWS_ATOMIC_STORE_HEAD_PTR(&ring_buf, ring_buf.allocation);
        AWS_ATOMIC_STORE_TAIL_PTR(&ring_buf, ring_buf.allocation);
    } else {
        uint8_t *head_ptr = ring_buf.allocation + (nondet_size_t() % alloc_size);
        uint8_t *tail_ptr = ring_buf.allocation + (nondet_size_t() % alloc_size);
        __CPROVER_assume(head_ptr != tail_ptr);
        AWS_ATOMIC_STORE_HEAD_PTR(&ring_buf, head_ptr);
        AWS_ATOMIC_STORE_TAIL_PTR(&ring_buf, tail_ptr);
    }

    /* ensure dest has an allocated buffer member for snapshotting */
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    /* dest must be a valid byte_buf before the call */
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* 2. Save old state BEFORE calling */
    uint8_t *old_head;
    uint8_t *old_tail;
    AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, old_head);
    AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, old_tail);
    struct aws_byte_buf old_dest = dest;

    /* precondition: requested_size != 0 */
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

    /* 4. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest must describe a buffer inside the ring allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
        assert(dest.len == requested_size);
        assert(dest.capacity == requested_size);
        assert(aws_byte_buf_is_valid(&dest));

        /* head must have advanced by exactly requested_size (modulo wrap) */
        size_t diff;
        if (new_head >= old_head) {
            diff = (size_t)(new_head - old_head);
        } else {
            diff = (size_t)(ring_buf.allocation_end - old_head) + (size_t)(new_head - ring_buf.allocation);
        }
        assert(diff == requested_size);

        /* tail must be unchanged */
        assert(new_tail == old_tail);
    } else {
        /* on failure, ring buffer must be unchanged */
        assert(new_head == old_head);
        assert(new_tail == old_tail);
        assert(ring_buf.allocation == ring_buf.allocation);
        assert(ring_buf.allocation_end == ring_buf.allocation_end);
        assert(ring_buf.allocator == allocator);

        /* dest must be unchanged */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
    }

    /* 5. Fields that must NOT change regardless of result */
    assert(ring_buf.allocation == ring_buf.allocation);
    assert(ring_buf.allocation_end == ring_buf.allocation_end);
    assert(ring_buf.allocator == allocator);

    /* 6. Ring buffer validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
