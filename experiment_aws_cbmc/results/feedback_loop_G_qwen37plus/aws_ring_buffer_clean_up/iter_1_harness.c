#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    
    /* Precondition: ring_buf must be a valid ring buffer before cleanup */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    
    /* Save old state */
    struct aws_allocator *old_allocator = ring_buf.allocator;
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    
    /* Call function under test */
    aws_ring_buffer_clean_up(&ring_buf);
    
    /* Postconditions: AWS_ZERO_STRUCT zeroes all fields */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
}
