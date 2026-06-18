#include <aws/common/common.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void aws_ring_buffer_acquire_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* ring buffer */
    struct aws_ring_buffer ring_buf;
    size_t ring_size;

    /* nondet size for the ring buffer, bounded to keep CBMC tractable */
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= 1024);

    /* initialize the ring buffer */
    int init_rc = aws_ring_buffer_init(&ring_buf, alloc, ring_size);
    __CPROVER_assume(init_rc == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* snapshot immutable fields */
    struct aws_allocator *alloc_before = ring_buf.allocator;
    uint8_t *allocation_before = ring_buf.allocation;
    uint8_t *allocation_end_before = ring_buf.allocation_end;

    /* snapshot allocation memory */
    size_t alloc_len = (size_t)(ring_buf.allocation_end - ring_buf.allocation);
    uint8_t *alloc_copy = malloc(alloc_len);
    __CPROVER_assume(alloc_copy != NULL);
    memcpy(alloc_copy, ring_buf.allocation, alloc_len);

    /* destination byte buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* requested size (non‑zero) */
    size_t requested_size;
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= 1024); /* bound for CBMC */

    /* call the function under verification */
    int rc = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 1. Return value / error code correctness */
    assert(rc == AWS_OP_SUCCESS || rc == AWS_OP_ERR);

    /* 2. Output buffer length/capacity invariants */
    if (rc == AWS_OP_SUCCESS) {
        /* aws_byte_buf_from_empty_array creates a buffer with len == 0 and capacity == requested_size */
        assert(dest.len == 0);
        assert(dest.capacity == requested_size);
        assert(dest.buffer != NULL);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
    } else {
        /* On error the function raises AWS_ERROR_OOM */
        assert(aws_last_error() == AWS_ERROR_OOM);
    }

    /* 3. Frame conditions: immutable fields must not change */
    assert(ring_buf.allocator == alloc_before);
    assert(ring_buf.allocation == allocation_before);
    assert(ring_buf.allocation_end == allocation_end_before);

    /* Allocation memory must remain unchanged */
    assert(memcmp(alloc_copy, ring_buf.allocation, alloc_len) == 0);

    /* clean up */
    free(alloc_copy);
    aws_ring_buffer_clean_up(&ring_buf);
    return 0;
}
