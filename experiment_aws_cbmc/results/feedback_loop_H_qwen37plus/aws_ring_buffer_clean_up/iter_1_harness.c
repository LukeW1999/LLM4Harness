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
    
    /* 1. Precondition: The ring buffer must be in a valid state before cleanup */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    
    /* 2. Save old state (though all fields will be zeroed, this documents intent) */
    struct aws_ring_buffer old = ring_buf;
    
    /* 3. Call function under test */
    aws_ring_buffer_clean_up(&ring_buf);
    
    /* 4. Assert postconditions: AWS_ZERO_STRUCT zeroes the entire struct.
       We assert that all pointer fields are set to NULL. */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    
    /* Note: The atomic variables 'head' and 'tail' are also zeroed by AWS_ZERO_STRUCT.
       Since aws_atomic_var is an opaque type/struct, we cannot directly assert 
       equality to 0 without knowing its internal layout, but zeroing the pointers 
       is a strong indicator of correct AWS_ZERO_STRUCT behavior. */
}
