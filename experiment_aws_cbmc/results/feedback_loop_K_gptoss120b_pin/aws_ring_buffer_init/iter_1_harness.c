/*  
 * Contract for aws_ring_buffer_init  
 * Preconditions:  
 *   - allocator != NULL (aws_default_allocator() provides a non‑null allocator)  
 *   - size > 0 (non‑zero allocation request)  
 *   - size is bounded to avoid overflow (e.g., size < SIZE_MAX/2)  
 *   - ring_buf points to a valid, writable memory region (stack‑allocated)  
 * Postconditions (validity):  
 *   - return value is either AWS_OP_SUCCESS or AWS_OP_ERR  
 *   - on AWS_OP_SUCCESS:  
 *       * ring_buf.allocation != NULL  
 *       * ring_buf.allocator == allocator  
 *       * head and tail atomic pointers both equal ring_buf.allocation  
 *       * ring_buf.allocation_end == ring_buf.allocation + size  
 *   - on AWS_OP_ERR:  
 *       * ring_buf.allocation == NULL  
 *       * ring_buf.allocator == NULL  
 *       * head and tail atomic pointers are NULL  
 *       * ring_buf.allocation_end == NULL  
 * Postconditions (frame):  
 *   - No memory outside of the fields of *ring_buf and the newly allocated buffer is modified.  
 */

#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Preconditions */
    __CPROVER_assume(alloc != NULL);

    size_t size = __CPROVER_nondet_size_t();
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size < (SIZE_MAX / 2));

    /* Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, alloc, size);

    /* Postconditions: return value */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* Allocation succeeded */
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocator == alloc);
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
        /* Bounds sanity */
        assert(ring_buf.allocation_end > ring_buf.allocation);
    } else {
        /* Allocation failed */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
        assert(ring_buf.allocation_end == NULL);
    }

    return 0;
}
