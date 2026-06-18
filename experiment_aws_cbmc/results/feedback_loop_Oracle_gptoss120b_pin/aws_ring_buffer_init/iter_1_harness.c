#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_ring_buffer *ring_buf_ptr = &ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size;

    /* nondet size */
    __CPROVER_assume(size > 0 && size < MAX_MALLOC);
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(ring_buf_ptr != NULL);

    /* preconditions */
    __CPROVER_assume(ring_buf_ptr != NULL);
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(size > 0 && size < MAX_MALLOC);

    int ret = aws_ring_buffer_init(ring_buf_ptr, allocator, size);

    /* postconditions */
    if (ret == AWS_OP_SUCCESS) {
        assert(ring_buf_ptr->allocation != NULL);
        assert(ring_buf_ptr->allocator == allocator);
        assert(aws_atomic_load_ptr(&ring_buf_ptr->head) == ring_buf_ptr->allocation);
        assert(aws_atomic_load_ptr(&ring_buf_ptr->tail) == ring_buf_ptr->allocation);
        assert(ring_buf_ptr->allocation_end == ring_buf_ptr->allocation + size);
        assert(aws_ring_buffer_is_valid(ring_buf_ptr));
    } else {
        assert(ret == AWS_OP_ERR);
        assert(ring_buf_ptr->allocation == NULL);
        assert(ring_buf_ptr->allocator == NULL);
        assert(aws_atomic_load_ptr(&ring_buf_ptr->head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf_ptr->tail) == NULL);
        assert(ring_buf_ptr->allocation_end == NULL);
        assert(!aws_ring_buffer_is_valid(ring_buf_ptr));
    }

    return 0;
}
