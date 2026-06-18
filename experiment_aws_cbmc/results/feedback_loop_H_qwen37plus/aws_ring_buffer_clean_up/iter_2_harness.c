#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0 && capacity < 1024);
    
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, capacity);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    
    aws_ring_buffer_clean_up(&ring_buf);
    
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
}
