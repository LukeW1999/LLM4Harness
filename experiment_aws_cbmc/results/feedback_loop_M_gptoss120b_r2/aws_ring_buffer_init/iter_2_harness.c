#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void aws_ring_buffer_init_harness() {
    /* Allocate and zero‑initialize the ring buffer structure */
    struct aws_ring_buffer ring_buf;
    memset(&ring_buf, 0, sizeof(ring_buf));

    /* Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Non‑deterministic size bounded by the global constant */
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    /* Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* Constrain to the successful path so that all assertions are reachable */
    __CPROVER_assume(result == AWS_OP_SUCCESS);

    /* Post‑condition checks */
    assert(ring_buf.allocation != NULL);
    assert(ring_buf.allocation_end == ring_buf.allocation + size);
    assert(ring_buf.allocator == allocator);
    assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
    assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);

    /* Invariant: the ring buffer must always be valid */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
