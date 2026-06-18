#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
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
    ring_buf.allocator = aws_default_allocator();
    
    bool has_alloc = nondet_bool();
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);
    
    if (has_alloc) {
        ring_buf.allocation = malloc(size);
        ring_buf.allocation_end = ring_buf.allocation + size;
    } else {
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
    }
    
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    aws_ring_buffer_clean_up(&ring_buf);

    /* Postconditions: AWS_ZERO_STRUCT zeroes out the entire structure */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
}
