#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness() {
    struct aws_ring_buffer ring_buf = {0};
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size = nondet_size_t();
    
    __CPROVER_assume(size > 0 && size <= 1024);

    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    if (result == AWS_OP_SUCCESS) {
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
        assert(aws_atomic_load_ptr(&ring_buf.head) == (void *)ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == (void *)ring_buf.allocation);
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation == NULL);
    }
}
