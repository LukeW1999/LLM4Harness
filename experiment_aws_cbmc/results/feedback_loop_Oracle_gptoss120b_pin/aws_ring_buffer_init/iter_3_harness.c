#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

void aws_ring_buffer_init_harness(void) {
    struct aws_ring_buffer ring_buf = {0};
    struct aws_ring_buffer *rb = &ring_buf;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t size = nondet_size_t();
    __CPROVER_assume(alloc != NULL);
    __CPROVER_assume(size > 0 && size < 1024 * 1024);

    int ret = aws_ring_buffer_init(rb, alloc, size);

    if (ret == AWS_OP_SUCCESS) {
        assert(rb->allocation != NULL);
        assert(rb->allocator == alloc);
        assert(aws_atomic_load_ptr(&rb->head) == rb->allocation);
        assert(aws_atomic_load_ptr(&rb->tail) == rb->allocation);
        assert(rb->allocation_end == (uint8_t *)rb->allocation + size);
        assert(aws_ring_buffer_is_valid(rb));
    } else {
        assert(ret == AWS_OP_ERR);
        assert(rb->allocation == NULL);
        assert(rb->allocator == NULL);
        assert(aws_atomic_load_ptr(&rb->head) == NULL);
        assert(aws_atomic_load_ptr(&rb->tail) == NULL);
        assert(rb->allocation_end == NULL);
        assert(!aws_ring_buffer_is_valid(rb));
    }
}
