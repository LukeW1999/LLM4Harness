#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/nondet.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness(void) {
    /* Allocate the ring buffer structure */
    struct aws_ring_buffer *ring_buf = malloc(sizeof(*ring_buf));
    __CPROVER_assume(ring_buf != NULL);

    /* Nondeterministic size (must be > 0) */
    size_t size = nondet_uint64_t();
    __CPROVER_assume(size > 0);

    /* Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Call the function under verification */
    int result = aws_ring_buffer_init(ring_buf, allocator, size);

    /* The function must return either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* The ring buffer must be correctly initialized */
        assert(ring_buf->allocator == allocator);
        assert(ring_buf->allocation != NULL);
        assert(ring_buf->allocation_size >= size);
        assert(ring_buf->allocation_end == ring_buf->allocation + ring_buf->allocation_size);
        assert(ring_buf->head == 0);
        assert(ring_buf->tail == 0);
        assert(ring_buf->size == size);
    } else {
        /* On error, at least the allocator and size arguments are unchanged */
        assert(allocator == allocator);
        assert(size == size);
        /* Trivial assertion to keep this branch reachable */
        assert(1);
    }
}
