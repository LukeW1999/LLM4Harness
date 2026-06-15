/*  
 * Contract for aws_ring_buffer_acquire  
 * Preconditions:  
 *   - ring_buf != NULL  
 *   - dest != NULL  
 *   - requested_size != 0  
 *   - aws_ring_buffer_is_valid(ring_buf) holds  
 *   - ring_buf->allocation != NULL implies ring_buf->allocation_end > ring_buf->allocation  
 *   - The atomic variables head and tail are initialized (as done by aws_ring_buffer_init)  
 *   - The allocator used for initialization is a valid allocator (aws_default_allocator())  
 *   - The size used to initialize the ring buffer is non‑zero and fits in size_t  
 * Postconditions (validity):  
 *   - The function returns either AWS_OP_SUCCESS (0) or a non‑zero error code (e.g., AWS_ERROR_OOM).  
 *   - If the return value is AWS_OP_SUCCESS, then dest->buffer points inside the ring buffer's allocation region,  
 *     dest->len == requested_size, and dest->capacity == requested_size.  
 *   - If the return value is an error, dest is left unchanged.  
 *   - In all cases, aws_ring_buffer_is_valid(ring_buf) still holds after the call.  
 * Postconditions (frame):  
 *   - No memory outside of ring_buf (including its allocation region) and dest is modified, except for the fields  
 *     head, tail, and the contents of dest as described above.  
 */

#include <aws/common/common.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_acquire_harness(void) {
    /* Allocate and nondet‑initialize a ring buffer */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet size for the ring buffer allocation, must be > 0 */
    size_t buf_size = nondet_size_t();
    __CPROVER_assume(buf_size > 0);

    /* Initialize the ring buffer */
    int init_ret = aws_ring_buffer_init(&ring_buf, alloc, buf_size);
    __CPROVER_assume(init_ret == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Allocate destination byte buffer */
    struct aws_byte_buf dest;
    /* nondet requested size, must be non‑zero */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);

    /* Save a copy of the ring buffer state for frame checking */
    struct aws_ring_buffer ring_buf_before = ring_buf;
    struct aws_byte_buf dest_before = dest;

    /* Call the function under test */
    int ret = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Postcondition: return value is either success or OOM error */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_ERROR_OOM);

    /* Postcondition: ring buffer remains valid */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    if (ret == AWS_OP_SUCCESS) {
        /* dest must describe a buffer of the requested size */
        assert(dest.len == requested_size);
        assert(dest.capacity == requested_size);
        /* dest->buffer must lie within the ring buffer allocation */
        assert(ring_buf.allocation != NULL);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
    } else {
        /* On error, dest must be unchanged */
        assert(dest.len == dest_before.len);
        assert(dest.capacity == dest_before.capacity);
        assert(dest.buffer == dest_before.buffer);
    }

    /* Frame conditions: only head, tail, and dest (when success) may be modified */
    /* Verify that allocation pointer and allocation_end are unchanged */
    assert(ring_buf.allocation == ring_buf_before.allocation);
    assert(ring_buf.allocation_end == ring_buf_before.allocation_end);
    /* Verify that the allocator pointer is unchanged */
    assert(ring_buf.allocator == ring_buf_before.allocator);
    /* Verify that other fields of the ring buffer (except head and tail) are unchanged */
    /* head and tail are atomic vars; we cannot directly compare their internal representation,
       but we can assert that the underlying pointer values are within expected bounds via
       aws_ring_buffer_check_atomic_ptr. */
    assert(aws_ring_buffer_check_atomic_ptr(&ring_buf, (uint8_t *)ring_buf.head));
    assert(aws_ring_buffer_check_atomic_ptr(&ring_buf, (uint8_t *)ring_buf.tail));

    return 0;
}
