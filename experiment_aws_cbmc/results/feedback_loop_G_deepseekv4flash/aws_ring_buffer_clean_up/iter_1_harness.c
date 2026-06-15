#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness() {
    /* Non-det allocation for the ring buffer structure */
    struct aws_ring_buffer *ring_buf = malloc(sizeof(*ring_buf));
    if (ring_buf == NULL) return;

    /* Assume the ring buffer is valid before cleanup */
    __CPROVER_assume(aws_ring_buffer_is_valid(ring_buf));

    /* Save the old state for comparisons (optional, but we assert zeroing) */
    struct aws_ring_buffer old = *ring_buf;

    /* Call the function under test */
    aws_ring_buffer_clean_up(ring_buf);

    /* After cleanup, the ring buffer must be zeroed */
    assert(ring_buf->allocator == NULL);
    assert(ring_buf->allocation == NULL);
    assert(ring_buf->allocation_end == NULL);
    /* atomic_var head and tail — they are structs containing a uint64_t.
       We can check that the underlying value is 0. */
    assert(aws_atomic_load_int(&ring_buf->head) == 0);
    assert(aws_atomic_load_int(&ring_buf->tail) == 0);
}
