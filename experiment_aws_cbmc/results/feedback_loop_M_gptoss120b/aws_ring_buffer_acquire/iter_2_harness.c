#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_acquire_harness(void) {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;

    /* nondet capacity bounded */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0 && capacity <= 1024);

    /* allocate the underlying buffer */
    ring_buf.allocation = (uint8_t *)aws_mem_acquire(allocator, capacity);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + capacity;
    ring_buf.size = capacity;

    /* initialise atomic head and tail to the start of the buffer (empty ring) */
    aws_atomic_store_ptr(&ring_buf.head, ring_buf.allocation);
    aws_atomic_store_ptr(&ring_buf.tail, ring_buf.allocation);

    /* assume the ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Declare and bound the destination byte buffer */
    struct aws_byte_buf dest;
    ensure_byte_buf_has_allocated_buffer_member(&dest);

    /* 3. Save old state */
    struct aws_ring_buffer old_ring = ring_buf;

    /* 4. Nondet request size, bounded and non‑zero (precondition) */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size > 0 && requested_size <= 1024);

    /* 5. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest must describe a buffer of the requested size */
        assert(dest.len == requested_size);
        /* The buffer must lie within the ring buffer allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);
        /* The buffer must belong to this ring buffer's pool */
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
    } else {
        /* On failure the ring buffer must be unchanged */
        assert(ring_buf.allocator == old_ring.allocator);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
        assert(aws_atomic_load_ptr(&ring_buf.head) == aws_atomic_load_ptr(&old_ring.head));
        assert(aws_atomic_load_ptr(&ring_buf.tail) == aws_atomic_load_ptr(&old_ring.tail));
    }

    /* 7. Fields that never change regardless of outcome */
    assert(ring_buf.allocator == old_ring.allocator);
    assert(ring_buf.allocation == old_ring.allocation);
    assert(ring_buf.allocation_end == old_ring.allocation_end);
    assert(ring_buf.size == old_ring.size);

    /* 8. Ring buffer validity invariant must hold */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
