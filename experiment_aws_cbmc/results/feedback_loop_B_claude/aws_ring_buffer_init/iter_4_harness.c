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
    __CPROVER_assume(allocator != NULL);

    /* Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* The result must be either success or error */
    __CPROVER_assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR,
                     "result is either success or error");

    /* Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(ring_buf.allocator == allocator,
                         "allocator is set correctly on success");
        __CPROVER_assert(ring_buf.allocation != NULL,
                         "allocation is non-null on success");
        __CPROVER_assert(ring_buf.allocation_end == ring_buf.allocation + size,
                         "allocation_end is set correctly on success");

        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
        __CPROVER_assert(head_ptr == ring_buf.allocation,
                         "head points to start of allocation on success");
        __CPROVER_assert(tail_ptr == ring_buf.allocation,
                         "tail points to start of allocation on success");

        __CPROVER_assert(aws_ring_buffer_is_valid(&ring_buf),
                         "ring buffer is valid on success");
    } else {
        __CPROVER_assert(result == AWS_OP_ERR,
                         "result is AWS_OP_ERR on failure");
    }
}
