#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness() {
    /* Non-deterministic ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Initialize to a valid ring buffer */
    ring_buf.allocator = can_fail_malloc_allocator();  /* nondet but valid allocator */
    ring_buf.allocation = malloc(sizeof(uint8_t)); /* nondet size, but assume non-null */
    size_t size;
    __CPROVER_assume(size > 0);
    ring_buf.allocation_end = ring_buf.allocation + size;

    /* Initialize head and tail to non-negative integers less than size */
    size_t head_val, tail_val;
    __CPROVER_assume(head_val < size);
    __CPROVER_assume(tail_val < size);
    aws_atomic_store_int(&ring_buf.head, head_val);
    aws_atomic_store_int(&ring_buf.tail, tail_val);

    /* Ensure validity (using the library's check) */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Save old state */
    struct aws_ring_buffer old_ring_buf = ring_buf;

    /* Call the function */
    aws_ring_buffer_clean_up(&ring_buf);

    /* Postconditions:
     * 1. The entire struct is zeroed out
     * 2. allocator, allocation, allocation_end are NULL
     * 3. head and tail are 0
     */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(aws_atomic_load_int(&ring_buf.head) == 0);
    assert(aws_atomic_load_int(&ring_buf.tail) == 0);

    /* After cleanup, the ring buffer is no longer valid */
    assert(!aws_ring_buffer_is_valid(&ring_buf));
}
