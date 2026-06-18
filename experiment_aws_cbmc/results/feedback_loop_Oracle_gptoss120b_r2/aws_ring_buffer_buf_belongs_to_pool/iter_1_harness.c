#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* Allocate and initialize a ring buffer */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t ring_size;
    __CPROVER_assume(ring_size > 0);
    uint8_t *mem = malloc(ring_size);
    __CPROVER_assume(mem != NULL);

    /* Initialise the ring buffer (sets allocator, allocation, allocation_end, head, tail) */
    int init_ret = aws_ring_buffer_init(&ring_buf, alloc, ring_size);
    __CPROVER_assume(init_ret == AWS_OP_SUCCESS);

    /* Make the ring buffer non‑empty */
    aws_atomic_store_int(&ring_buf.head, 0);
    aws_atomic_store_int(&ring_buf.tail, 1);

    /* Assume the structural preconditions */
    __CPROVER_assume(!aws_ring_buffer_is_empty(&ring_buf));
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Create a byte buffer */
    struct aws_byte_buf buf;
    make_aws_byte_buf(&buf, alloc);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save copies for frame condition checks */
    struct aws_ring_buffer ring_buf_old = ring_buf;
    struct aws_byte_buf   buf_old      = buf;

    /* Call the function under verification */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    /* -------------------------------------------------------------------- */
    /* Post‑condition 1: Return value must be a boolean (always true/false) */
    __CPROVER_assert(result == true || result == false,
                     "Result of aws_ring_buffer_buf_belongs_to_pool is a boolean");

    /* Post‑condition 2: If the result is true, the buffer must lie within the pool */
    __CPROVER_assert(
        result == (buf.buffer >= ring_buf.allocation && buf.buffer < ring_buf.allocation_end),
        "Result matches membership of buf.buffer in ring buffer allocation");

    /* Post‑condition 3: Frame condition – ring buffer must be unchanged */
    __CPROVER_assert(
        memcmp(&ring_buf, &ring_buf_old, sizeof(ring_buf)) == 0,
        "aws_ring_buffer_buf_belongs_to_pool must not modify the ring buffer");

    /* Post‑condition 4: Frame condition – byte buffer must be unchanged */
    __CPROVER_assert(
        memcmp(&buf, &buf_old, sizeof(buf)) == 0,
        "aws_ring_buffer_buf_belongs_to_pool must not modify the byte buffer");

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buf);
    free(mem);
    return 0;
}
