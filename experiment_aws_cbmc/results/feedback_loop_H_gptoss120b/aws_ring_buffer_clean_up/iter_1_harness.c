#include <aws/common/ring_buffer.h>
#include <assert.h>

/* CBMC harness for aws_ring_buffer_clean_up */
void aws_ring_buffer_clean_up_harness(void) {
    /* 1. Declare the ring buffer */
    struct aws_ring_buffer ring_buf;

    /* 2. Assume the ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 3. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 4. Post‑conditions: all fields must be zeroed */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.head == (struct aws_atomic_var){0});
    assert(ring_buf.tail == (struct aws_atomic_var){0});

    /* 5. The ring buffer must remain a valid (empty) structure */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
