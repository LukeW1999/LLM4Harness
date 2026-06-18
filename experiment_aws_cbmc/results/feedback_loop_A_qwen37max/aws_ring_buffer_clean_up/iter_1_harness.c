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
    
    /* Assume the ring buffer is valid before cleanup */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    
    /* Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);
    
    /* Postconditions: the struct is completely zeroed out */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
}
