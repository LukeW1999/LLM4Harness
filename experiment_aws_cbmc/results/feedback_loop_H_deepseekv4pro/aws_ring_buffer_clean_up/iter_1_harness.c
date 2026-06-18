#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;

    /* Initialize allocator to a valid default allocator */
    ring_buf.allocator = aws_default_allocator();

    /* Non-deterministically allocate memory for the ring buffer's backing store */
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);
    ring_buf.allocation = (uint8_t *)malloc(size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.all
