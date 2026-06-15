#include <aws/common/ring_buffer.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_ring_buffer_clean_up_harness() {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring_buf;
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save old state */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 4. Post‑conditions: all fields must be zeroed */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.head == (struct aws_atomic_var){0});
    assert(ring_buf.tail == (struct aws_atomic_var){0});

    /* 5. Unchanged fields – none, everything is expected to be zero */

    /* 6. Validity invariant must still hold after clean‑up */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
