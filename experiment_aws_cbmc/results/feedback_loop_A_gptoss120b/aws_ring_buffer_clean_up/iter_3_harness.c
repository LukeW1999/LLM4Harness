#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <assert.h>

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Choose a bounded, non‑zero size for the buffer */
    size_t size;
    __CPROVER_assume(size > 0 && size < 1024);

    /* Force successful initialisation so the post‑conditions are reachable */
    __CPROVER_assume(aws_ring_buffer_init(&ring_buf, alloc, size) == AWS_OP_SUCCESS);

    /* Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* Post‑conditions */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.head.value == 0);
    assert(ring_buf.tail.value == 0);
    assert(!aws_ring_buffer_is_valid(&ring_buf));
}
