#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/nondet.h"
#include "proof_helpers/utils.h"

void aws_ring_buffer_init_harness(void) {
    /* Allocate the ring buffer structure */
    struct aws_ring_buffer *ring_buf = malloc(sizeof(*ring_buf));
    __CPROVER_assume(ring_buf != NULL);

    /* Preserve a copy for frame checks */
    struct aws_ring_buffer old_ring_buf = *ring_buf;

    /* Use a nondeterministic default allocator */
    struct aws_allocator *alloc = make_default_allocator();
    __CPROVER_assume(alloc != NULL);

    /* Nondeterministic size with reasonable bounds */
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size < (1U << 20)); /* < 1 MiB */

    /* Call the function under test */
    int ret = aws_ring_buffer_init(ring_buf, alloc, size);

    if (ret == AWS_OP_SUCCESS) {
        /* Success post‑conditions */
        assert(ring_buf->allocation != NULL);
        assert(ring_buf->allocator == alloc);
        assert(ring_buf->allocation_end == (uint8_t *)ring_buf->allocation + size);
        assert(aws_atomic_load_ptr(&ring_buf->head) == ring_buf->allocation);
        assert(aws_atomic_load_ptr(&ring_buf->tail) == ring_buf->allocation);
    } else {
        /* Failure post‑conditions */
        assert(ret == AWS_OP_ERR);
        assert(ring_buf->allocation == NULL);
        assert(ring_buf->allocator == NULL);
        assert(ring_buf->allocation_end == NULL);
        assert(aws_atomic_load_ptr(&ring_buf->head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf->tail) == NULL);

        /* Frame conditions when init fails */
        assert(ring_buf->allocation == old_ring_buf.allocation);
        assert(ring_buf->allocator == old_ring_buf.allocator);
        assert(ring_buf->allocation_end == old_ring_buf.allocation_end);
        assert(aws_atomic_load_ptr(&ring_buf->head) == aws_atomic_load_ptr(&old_ring_buf.head));
        assert(aws_atomic_load_ptr(&ring_buf->tail) == aws_atomic_load_ptr(&old_ring_buf.tail));
    }

    /* Frame conditions for unchanged arguments */
    assert(alloc == make_default_allocator());
    (void)size; /* size is not modified; silence unused‑variable warnings */

    /* Clean up */
    if (ring_buf->allocation != NULL) {
        aws_mem_release(ring_buf->allocator, ring_buf->allocation);
    }
    free(ring_buf);
}
