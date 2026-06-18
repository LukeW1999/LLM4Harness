#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    size_t size;

    /* nondeterministic size, bounded to avoid overflow */
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size < 1024 * 1024);

    /* initialize the ring buffer */
    int init_res = aws_ring_buffer_init(&ring_buf, aws_default_allocator(), size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* call the function under verification */
    aws_ring_buffer_clean_up(&ring_buf);

    /* post‑conditions */
    __CPROVER_assert(ring_buf.allocation == NULL,
                     "aws_ring_buffer_clean_up: allocation pointer must be NULL after clean up");
    __CPROVER_assert(ring_buf.allocator == NULL,
                     "aws_ring_buffer_clean_up: allocator pointer must be NULL after clean up");
    __CPROVER_assert(ring_buf.allocation_end == NULL,
                     "aws_ring_buffer_clean_up: allocation_end pointer must be NULL after clean up");
    __CPROVER_assert(aws_atomic_load_int(&ring_buf.head) == 0,
                     "aws_ring_buffer_clean_up: head atomic must be zero after clean up");
    __CPROVER_assert(aws_atomic_load_int(&ring_buf.tail) == 0,
                     "aws_ring_buffer_clean_up: tail atomic must be zero after clean up");

    return 0;
}
