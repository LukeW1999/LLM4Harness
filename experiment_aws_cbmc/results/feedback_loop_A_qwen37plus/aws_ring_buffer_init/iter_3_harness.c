#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size = nondet_size_t();
    
    __CPROVER_assume(size > 0 && size <= 1024);

    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    assert(result == AWS_OP_SUCCESS);
    assert(ring_buf.allocator == allocator);
    assert(ring_buf.allocation != NULL);
    assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
    assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
    assert(ring_buf.allocation_end == ring_buf.allocation + size);
}
