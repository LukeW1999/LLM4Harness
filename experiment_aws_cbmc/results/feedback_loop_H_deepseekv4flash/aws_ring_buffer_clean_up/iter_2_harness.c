#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness() {
    /* Non-deterministic ring buffer with valid pre-state */
    struct aws_ring_buffer ring_buf;
    struct aws_ring_buffer old_ring_buf;

    /* Create a valid allocator using helper */
    ring_buf.allocator = nondet_bool() ? NULL : can_fail_allocator();
    __CPROVER_assume(ring_buf.allocator != NULL);

    /* Allocate non-det memory for the buffer (must be non-null for a valid ring buffer) */
    size_t size;
    __CPROVER_assume(size > 0);
    uint8_t *allocation = malloc(size);
    __CPROVER_assume(allocation != NULL);
    ring_buf.allocation = allocation;
    ring_buf.allocation_end = allocation + size;

    /* Initialize atomic head and tail to point to the start of the buffer */
    aws_atomic_store_ptr(&ring_buf.head, allocation);
    aws_atomic_store_ptr(&ring_buf.tail, allocation);

    /* Save old state for comparison */
    old_ring_buf = ring_buf;

    /* Call the function under verification */
    aws_ring_buffer_clean_up(&ring_buf);

    /* Postconditions:
     * 1. All struct fields are zeroed (AWS_ZERO_STRUCT behavior)
     * 2. The allocation has been freed (modeled as NULL)
     */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
    assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
}
