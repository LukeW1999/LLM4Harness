#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_MALLOC (SIZE_MAX / 2)

void aws_ring_buffer_init_harness(void) {
    /* Setup */
    struct aws_ring_buffer *ring_buf = malloc(sizeof(struct aws_ring_buffer));
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size;

    /* Ground-truth preconditions */
    __CPROVER_assume(ring_buf != NULL);
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(size > 0 && size < MAX_MALLOC);

    /* Call the function under test */
    int result = aws_ring_buffer_init(ring_buf, allocator, size);

    /* Postcondition 1: Return value correctness */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* Postcondition 2: On success, the ring buffer must be valid */
    if (result == AWS_OP_SUCCESS) {
        /* The allocator must be set correctly */
        assert(ring_buf->allocator == allocator);

        /* The allocation must be non-NULL */
        assert(ring_buf->allocation != NULL);

        /* allocation_end must be allocation + size */
        assert(ring_buf->allocation_end == ring_buf->allocation + size);

        /* allocation_end must be strictly greater than allocation (size > 0) */
        assert(ring_buf->allocation_end > ring_buf->allocation);

        /* Head and tail atomics must point to the start of the allocation */
        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf->head);
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf->tail);
        assert(head_ptr == ring_buf->allocation);
        assert(tail_ptr == ring_buf->allocation);

        /* Head and tail must be equal (ring buffer is empty) */
        assert(head_ptr == tail_ptr);

        /* The ring buffer must satisfy the validity predicate */
        assert(aws_ring_buffer_is_valid(ring_buf));

        /* The ring buffer must be empty */
        assert(aws_ring_buffer_is_empty(ring_buf));

        /* Frame condition: allocation is within bounds */
        assert(ring_buf->allocation_end >= ring_buf->allocation);
    }

    /* Postcondition 3: On failure, allocation should be NULL */
    if (result == AWS_OP_ERR) {
        assert(ring_buf->allocation == NULL);
        assert(ring_buf->allocator == NULL);
        assert(ring_buf->allocation_end == NULL);
    }
}
