#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);
    
    size_t size;
    __CPROVER_assume(size > 0 && size < 1024 * 1024);
    
    if (aws_ring_buffer_init(&ring_buf, allocator, size) == AWS_OP_SUCCESS) {
        aws_ring_buffer_clean_up(&ring_buf);
        
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocation_end == NULL);
    }
}
