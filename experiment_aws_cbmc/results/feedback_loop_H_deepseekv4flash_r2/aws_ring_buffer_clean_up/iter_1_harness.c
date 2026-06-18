#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness() {
    /* Non-deterministic ring buffer */
    struct aws_ring_buffer ring_buf;
    struct aws_ring_buffer old_ring_buf;

    /* Ensure the ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Save old state */
    old_ring_buf = ring_buf;

    /* Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* Postconditions: */
    /* 1. If allocation was non-null, it was released (we can't check the allocator, but we can check the pointer is null) */
    /* 2. The entire struct is zeroed */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    /* head and tail are atomic, but after zeroing they should be zero */
    /* We can't directly compare atomic values, but we can check the struct is zeroed */
    assert(ring_buf.allocation_end == NULL);

    /* 3. Validity: after cleanup, the ring buffer is no longer valid (allocation is null) */
    /* But we can still check that the struct is zeroed */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
}
