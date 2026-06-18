#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void aws_ring_buffer_init_harness() {
    /* 1. Declare the ring buffer structure */
    struct aws_ring_buffer ring_buf;
    struct aws_ring_buffer old = ring_buf; /* save old state (not used after zeroing) */

    /* 2. Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 3. Non‑deterministic size bounded by MAX_BUFFER_SIZE */
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    /* 4. Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success: allocation succeeded and fields are set */
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocation_end == (uint8_t *)ring_buf.allocation + size);
        assert(ring_buf.allocator == allocator);
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
    } else {
        /* Failure: allocation failed, all fields remain NULL/zero */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(ring_buf.allocator == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
    }

    /* 6. Validity invariant must always hold */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
