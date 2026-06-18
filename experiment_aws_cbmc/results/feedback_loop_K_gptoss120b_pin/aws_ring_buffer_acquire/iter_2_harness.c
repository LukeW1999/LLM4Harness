/*
 * CBMC harness for aws_ring_buffer_acquire
 */

#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stddef.h>

void aws_ring_buffer_acquire_harness(void) {
    /* Allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Ring buffer */
    struct aws_ring_buffer ring_buf;
    __CPROVER_assume(&ring_buf != NULL);

    /* Allocation size for the ring buffer (must be > 0) */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0);

    /* Initialize the ring buffer */
    int init_res = aws_ring_buffer_init(&ring_buf, allocator, alloc_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Destination byte buffer – must be a valid, empty buffer before the call */
    struct aws_byte_buf dest;
    dest.buffer   = NULL;
    dest.len      = 0;
    dest.capacity = 0;
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* Preserve original dest for the error case */
    struct aws_byte_buf dest_old = dest;

    /* Requested size – non‑zero and not larger than the ring buffer allocation */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= alloc_size);

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
        assert(dest.buffer   == dest_old.buffer);
        assert(dest.len      == dest_old.len);
        assert(dest.capacity == dest_old.capacity);
    }

    /* Ring buffer must remain valid after the call */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buf);
}
