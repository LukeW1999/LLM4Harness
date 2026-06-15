#include <aws/common/ring_buffer.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_ring_buffer_clean_up_harness() {
    /* 1. Declare the ring buffer and assume it is initially valid */
    struct aws_ring_buffer ring_buf;
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save the old state for comparison */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 4. Post‑conditions: all fields must be zero after clean‑up */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation_end == NULL);

    /* atomic variables are zeroed – the internal representation uses a
       `value` member (size_t) in the AWS implementation */
    assert(ring_buf.head.value == 0);
    assert(ring_buf.tail.value == 0);

    /* 5. The ring buffer is no longer a valid, initialized buffer */
    assert(!aws_ring_buffer_is_valid(&ring_buf));
}
