#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <assert.h>

/* CBMC harness for aws_ring_buffer_clean_up */
void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *alloc = aws_default_allocator();

    /* allocate a power‑of‑two sized buffer using the allocator */
    const size_t buf_size = 64; /* power of two */
    uint8_t *buf = alloc->mem_acquire(alloc, buf_size);
    __CPROVER_assume(buf != NULL);

    /* initialize the ring buffer fields */
    ring_buf.allocator      = alloc;
    ring_buf.allocation     = buf;
    ring_buf.allocation_end = buf + buf_size;
    ring_buf.head           = (struct aws_atomic_var){0};
    ring_buf.tail           = (struct aws_atomic_var){0};
    ring_buf.size           = buf_size;
    ring_buf.mask           = buf_size - 1;

    /* assume the ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* post‑conditions: all fields must be zeroed */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.head == (struct aws_atomic_var){0});
    assert(ring_buf.tail == (struct aws_atomic_var){0});
}
