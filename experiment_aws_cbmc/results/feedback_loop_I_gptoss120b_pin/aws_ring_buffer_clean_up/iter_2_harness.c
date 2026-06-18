#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;

    /* allocate a valid allocator */
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);
    __CPROVER_assume(alloc->mem_release != NULL);
    ring_buf.allocator = alloc;

    /* allocate a non‑zero sized buffer for the ring */
    size_t size;
    __CPROVER_assume(size > 0 && size < 1024);
    ring_buf.allocation = alloc->mem_acquire(alloc, size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = (uint8_t *)ring_buf.allocation + size;

    /* assume the ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* post‑conditions */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);

    {
        struct aws_atomic_var zero_atomic = { 0 };
        assert(ring_buf.head == zero_atomic);
        assert(ring_buf.tail == zero_atomic);
    }

    /* the zeroed ring buffer must still be considered valid */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
