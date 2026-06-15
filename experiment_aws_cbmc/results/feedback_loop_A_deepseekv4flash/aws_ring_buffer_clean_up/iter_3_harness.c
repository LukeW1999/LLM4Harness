#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness() {
    /* Non-deterministic ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Initialize allocator (non-null for valid ring buffer) */
    ring_buf.allocator = nondet_void_pointer();
    __CPROVER_assume(ring_buf.allocator != NULL);

    /* Allocate memory for the ring buffer */
    size_t size;
    __CPROVER_assume(size > 0);
    ring_buf.allocation = malloc(size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + size;

    /* Initialize head and tail (must be within [0, size) ) */
    size_t head_val, tail_val;
    __CPROVER_assume(head_val >= 0 && head_val < size);
    __CPROVER_assume(tail_val >= 0 && tail_val < size);
    aws_atomic_store_int(&ring_buf.head, head_val);
    aws_atomic_store_int(&ring_buf.tail, tail_val);

    /* Ensure the ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* Postconditions: everything zeroed and ring buffer invalid */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(aws_atomic_load_int(&ring_buf.head) == 0);
    assert(aws_atomic_load_int(&ring_buf.tail) == 0);
    assert(!aws_ring_buffer_is_valid(&ring_buf));
}
