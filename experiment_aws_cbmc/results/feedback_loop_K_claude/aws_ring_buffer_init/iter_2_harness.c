#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness(void) {
    /* Allocate ring buffer on the stack */
    struct aws_ring_buffer ring_buf;

    /* Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    assert(allocator != NULL);

    /* Precondition: size is non-zero and bounded for tractability */
    size_t size;
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size <= 1024);

    /* Call the function under verification */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* Postcondition: return value is either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* Postcondition: allocation is non-null */
        assert(ring_buf.allocation != NULL);

        /* Postcondition: allocator is set correctly */
        assert(ring_buf.allocator == allocator);

        /* Postcondition: allocation_end == allocation + size */
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        /* Postcondition: head and tail atomics point to allocation start */
        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
        assert(head_ptr == ring_buf.allocation);
        assert(tail_ptr == ring_buf.allocation);

        /* Postcondition: ring buffer satisfies validity invariant */
        assert(aws_ring_buffer_is_valid(&ring_buf));

        /* Postcondition: ring buffer is empty after init */
        assert(aws_ring_buffer_is_empty(&ring_buf));

        /* Clean up to avoid memory leaks */
        aws_ring_buffer_clean_up(&ring_buf);
    }
}
