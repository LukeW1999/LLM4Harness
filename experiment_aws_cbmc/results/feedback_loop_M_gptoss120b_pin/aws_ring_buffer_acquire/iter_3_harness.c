#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/memory.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 1024U

/* Helper to nondeterministically allocate a buffer for the ring */
static void allocate_ring_buffer(struct aws_ring_buffer *ring_buf, size_t capacity) {
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf->allocation = (uint8_t *)aws_mem_acquire(allocator, capacity);
    __CPROVER_assume(ring_buf->allocation != NULL);
    ring_buf->allocation_end = ring_buf->allocation + capacity;

    /* start with empty buffer (head == tail) */
    AWS_ATOMIC_STORE_HEAD_PTR(ring_buf, ring_buf->allocation);
    AWS_ATOMIC_STORE_TAIL_PTR(ring_buf, ring_buf->allocation);

    ring_buf->allocator = allocator;
}

/* Helper to read the atomic pointer values (implementation‑specific) */
static uint8_t *atomic_head_ptr(const struct aws_ring_buffer *ring_buf) {
    uint8_t *ptr;
    AWS_ATOMIC_LOAD_HEAD_PTR((struct aws_ring_buffer *)ring_buf, ptr);
    return ptr;
}
static uint8_t *atomic_tail_ptr(const struct aws_ring_buffer *ring_buf) {
    uint8_t *ptr;
    AWS_ATOMIC_LOAD_TAIL_PTR((struct aws_ring_buffer *)ring_buf, ptr);
    return ptr;
}

void aws_ring_buffer_acquire_harness(void) {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring_buf;
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0 && capacity <= MAX_BUFFER_SIZE);
    allocate_ring_buffer(&ring_buf, capacity);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Declare the destination byte buffer (will be overwritten) */
    struct aws_byte_buf dest;

    /* 3. Nondeterministic requested size, bounded */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size > 0 && requested_size <= MAX_BUFFER_SIZE);

    /* 4. Save old state for immutability checks */
    struct aws_ring_buffer old_ring = ring_buf;
    uint8_t *old_head = atomic_head_ptr(&old_ring);
    uint8_t *old_tail = atomic_tail_ptr(&old_ring);
    struct aws_byte_buf old_dest = dest; /* uninitialized, saved for completeness */

    /* 5. Call the function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 6. Post‑condition checks */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    if (result == AWS_OP_SUCCESS) {
        /* The returned buffer must lie inside the ring allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
        /* The capacity of the returned buffer must equal the requested size */
        assert(dest.capacity == requested_size);
        /* The length of the returned buffer is initially zero */
        assert(dest.len == 0);

        /* The head pointer must have advanced by the requested size (with wrap‑around) */
        uint8_t *new_head = atomic_head_ptr(&ring_buf);
        assert(new_head == old_head + requested_size ||
               (old_head + requested_size > ring_buf.allocation_end &&
                new_head == ring_buf.allocation + (old_head + requested_size - ring_buf.allocation_end)));

        /* The tail pointer must be unchanged on success */
        assert(atomic_tail_ptr(&ring_buf) == old_tail);
    } else {
        /* On failure the ring buffer must be unchanged */
        assert(atomic_head_ptr(&ring_buf) == old_head);
        assert(atomic_tail_ptr(&ring_buf) == old_tail);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
        assert(ring_buf.allocator == old_ring.allocator);
        /* Destination buffer must remain unchanged */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.len == old_dest.len);
    }

    /* Fields that never change regardless of outcome */
    assert(ring_buf.allocator == old_ring.allocator);
    assert(ring_buf.allocation == old_ring.allocation);
    assert(ring_buf.allocation_end == old_ring.allocation_end);
}
