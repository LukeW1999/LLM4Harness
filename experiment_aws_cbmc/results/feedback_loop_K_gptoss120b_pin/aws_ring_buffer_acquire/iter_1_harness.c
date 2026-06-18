/*  
Contract for aws_ring_buffer_acquire  

Preconditions:  
- ring_buf != NULL  
- dest != NULL  
- requested_size != 0 (AWS_ERROR_PRECONDITION)  
- ring_buf points to a valid, initialized aws_ring_buffer (aws_ring_buffer_is_valid(ring_buf) == true)  
- The atomic head and tail pointers inside ring_buf are within the allocation bounds (aws_ring_buffer_check_atomic_ptr(ring_buf, head) && aws_ring_buffer_check_atomic_ptr(ring_buf, tail))  

Postconditions (validity):  
- The function returns either AWS_OP_SUCCESS or AWS_OP_ERR.  
- If the return value is AWS_OP_SUCCESS, then:  
  * dest->buffer points to a memory region that belongs to ring_buf (aws_ring_buffer_buf_belongs_to_pool(ring_buf, dest) == true)  
  * dest->len == requested_size  
  * dest->capacity == requested_size  
- If the return value is AWS_OP_ERR, then dest is left unchanged (no assumptions on its contents).  

Postconditions (length):  
- The total used space in the ring buffer never exceeds its allocation size.  

Postconditions (frame):  
- No memory outside of ring_buf, its allocation, and dest is modified.  
- The allocator state is unchanged.  
*/

#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_acquire_harness(void) {
    /* Allocate and initialize allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Allocate ring buffer */
    struct aws_ring_buffer ring_buf;
    __CPROVER_assume(&ring_buf != NULL);

    /* Nondeterministic size for the underlying allocation (must be > 0) */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0);

    /* Initialize the ring buffer */
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, alloc_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Allocate destination byte buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(&dest != NULL);
    /* Preserve original dest for frame condition */
    struct aws_byte_buf dest_old = dest;

    /* Nondeterministic requested size (must be non‑zero) */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);

    /* Call the function under verification */
    int ret = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Return value must be either success or error */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    if (ret == AWS_OP_SUCCESS) {
        /* Destination must belong to the ring buffer's allocation */
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));

        /* Length and capacity must match the requested size */
        assert(dest.len == requested_size);
        assert(dest.capacity == requested_size);
    } else {
        /* On error, dest must be unchanged */
        assert(dest.buffer == dest_old.buffer);
        assert(dest.len == dest_old.len);
        assert(dest.capacity == dest_old.capacity);
    }

    /* Ring buffer must remain valid after the call */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    /* The head and tail pointers must still be within bounds */
    uint8_t *head_ptr = NULL;
    uint8_t *tail_ptr = NULL;
    AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, head_ptr);
    AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, tail_ptr);
    assert(aws_ring_buffer_check_atomic_ptr(&ring_buf, head_ptr));
    assert(aws_ring_buffer_check_atomic_ptr(&ring_buf, tail_ptr));

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buf);
    return 0;
}
