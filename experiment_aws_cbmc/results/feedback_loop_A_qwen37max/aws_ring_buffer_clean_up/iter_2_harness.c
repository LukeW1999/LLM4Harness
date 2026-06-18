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
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_ring_buffer ring_buf;
    
    ring_buf.allocator = allocator;
    
    size_t size;
    __CPROVER_assume(size > 0 && size <= 100);
    ring_buf.allocation = malloc(size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + size;
    
    /* Assume the ring buffer is valid before cleanup */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    
    /* Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);
    
    /* Postconditions: the struct is completely zeroed out */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
}
