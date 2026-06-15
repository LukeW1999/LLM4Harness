#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness() {
    /* Non-deterministic ring buffer */
    struct aws_ring_buffer ring_buf;
    struct aws_ring_buffer old_ring_buf;

    /* Ensure the ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Save old state */
    old_ring_buf = ring_buf;

    /* Call the function */
    aws_ring_buffer_clean_up(&ring_buf);

    /* Postconditions:
     * 1. If allocation was non-null, it was released (we can't check that directly, but we can check the result)
     * 2. The ring buffer is zeroed out (AWS_ZERO_STRUCT)
     * 3. The ring buffer is still valid (zeroed struct is valid)
     */

    /* Check that the ring buffer is zeroed */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL2);
    /* Atomic variables are zeroed (head and tail) */
    assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
    assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);

    /* Check that the ring buffer is still valid */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
