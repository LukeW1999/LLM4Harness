#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <stdint.h>
#include <stddef.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_ring_buffer_init_harness(void) {
    /* Declare inputs */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator;
    size_t size;

    /* Use a non-deterministic allocator that can fail */
    allocator = can_fail_allocator();

    /* Bound the size to keep the state space manageable */
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    /* Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* Assert postconditions - both branches must be reachable */
    if (result == AWS_OP_SUCCESS) {
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
        assert(head_ptr == ring_buf.allocation);
        assert(tail_ptr == ring_buf.allocation);

        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        assert(result == AWS_OP_ERR);
        assert(ring_buf.allocation == NULL);
    }
}
