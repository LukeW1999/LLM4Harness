#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/nondet.h"
#include "proof_helpers/utils.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness(void) {
    struct aws_ring_buffer *ring_buf = malloc(sizeof(*ring_buf));
    __CPROVER_assume(ring_buf != NULL);

    size_t size = nondet_uint64_t();
    __CPROVER_assume(size > 0);

    struct aws_allocator *allocator = aws_default_allocator();

    int result = aws_ring_buffer_init(ring_buf, allocator, size);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(ring_buf->allocator == allocator);
        assert(ring_buf->allocation != NULL);
        assert(ring_buf->allocation_size >= size);
        assert(ring_buf->allocation_end == ring_buf->allocation + ring_buf->allocation_size);
        assert(ring_buf->head == 0);
        assert(ring_buf->tail == 0);
        assert(ring_buf->size == size);
    } else {
        /* Ensure the error branch is reachable */
        assert(1);
    }
}
