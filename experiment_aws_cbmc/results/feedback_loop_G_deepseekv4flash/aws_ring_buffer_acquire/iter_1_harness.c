#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

#define MAX_RING_BUFFER_SIZE 256

void aws_ring_buffer_acquire_harness() {
    /* Non-deterministic inputs */
    struct aws_ring_buffer ring_buf;
    size_t requested_size;
    struct aws_byte_buf dest;

    /* Assume valid ring buffer */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    __CPROVER_assume(ring_buf.allocation != NULL);
    __CPROVER_assume(ring_buf.allocation_end != NULL);
    __CPROVER_assume(ring_buf.allocation_end > ring_buf.allocation);
    __CPROVER_assume((size_t)(ring_buf.allocation_end - ring_buf.allocation) <= MAX_RING_BUFFER_SIZE);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(ring_buf.allocation, ring_buf.allocation_end - ring_buf.allocation));

    /* Assume dest is a valid pointer to a writable struct */
    __CPROVER_assume(dest.allocator == NULL || aws_allocator_is_valid(dest.allocator));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&dest, sizeof(dest)));

    /* Assume requested_size is positive and bounded */
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size <= MAX_RING_BUFFER_SIZE);

    /* Save old state */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_dest = dest;

    /* Call the function */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest is updated */
        assert(dest.capacity == requested_size);
        assert(dest.len == 0);
        assert(dest.allocator == NULL);
        /* dest.buffer must point within the ring buffer's allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
        /* ring buffer head pointer advanced (cannot directly compare atomic, but we can check validity) */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* On failure, ring buffer state unchanged */
        assert(ring_buf.allocator == old_ring_buf.allocator);
        assert(ring_buf.allocation == old_ring_buf.allocation);
        assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
        /* Atomic head and tail: we compare the underlying pointer values by casting */
        /* Note: This assumes aws_atomic_var stores a pointer as its first member */
        uint8_t *old_head = *(uint8_t **)&old_ring_buf.head;
        uint8_t *old_tail = *(uint8_t **)&old_ring_buf.tail;
        uint8_t *new_head = *(uint8_t **)&ring_buf.head;
        uint8_t *new_tail = *(uint8_t **)&ring_buf.tail;
        assert(old_head == new_head);
        assert(old_tail == new_tail);
        /* dest unchanged */
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.len == old_dest.len);
        /* ring buffer still valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }

    /* Always: ring buffer validity */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
