#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/nondet.h"
#include "proof_helpers/utils.h"

#define MAX_MALLOC 1048576U /* 1 MiB */

void aws_ring_buffer_init_harness(void) {
    /* Allocate the ring buffer structure */
    struct aws_ring_buffer *ring_buf = malloc(sizeof(*ring_buf));
    __CPROVER_assume(ring_buf != NULL);

    /* Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* Nondeterministic size within reasonable bounds */
    size_t size = nondet_size_t();
    __CPROVER_assume(size <= MAX_MALLOC);

    /* Save a copy of the original structure for frame‑condition checking */
    struct aws_ring_buffer old_ring_buf;
    memcpy(&old_ring_buf, ring_buf, sizeof(*ring_buf));

    /* Call the function under verification */
    int ret = aws_ring_buffer_init(ring_buf, allocator, size);

    /* Post‑condition checks */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    if (ret == AWS_OP_SUCCESS) {
        /* Allocation must have succeeded */
        assert(ring_buf->allocation != NULL);
        /* Allocator field must be set to the provided allocator */
        assert(ring_buf->allocator == allocator);
        /* Head and tail atomic pointers must point to the start of the allocation */
        assert(aws_atomic_load_ptr(&ring_buf->head) == ring_buf->allocation);
        assert(aws_atomic_load_ptr(&ring_buf->tail) == ring_buf->allocation);
        /* allocation_end must be correctly computed */
        assert(ring_buf->allocation_end == (uint8_t *)ring_buf->allocation + size);
    } else {
        /* On error the allocation must be NULL */
        assert(ring_buf->allocation == NULL);
        /* All other fields must remain zero (as a result of AWS_ZERO_STRUCT) */
        assert(ring_buf->allocator == NULL);
        assert(aws_atomic_load_ptr(&ring_buf->head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf->tail) == NULL);
        assert(ring_buf->allocation_end == NULL);
    }

    /* Frame condition: only the ring buffer structure may be modified */
    assert(memcmp(&old_ring_buf, ring_buf, sizeof(*ring_buf)) == 0 ||
           (ret == AWS_OP_SUCCESS &&
            ring_buf->allocation != NULL &&
            ring_buf->allocator == allocator &&
            aws_atomic_load_ptr(&ring_buf->head) == ring_buf->allocation &&
            aws_atomic_load_ptr(&ring_buf->tail) == ring_buf->allocation &&
            ring_buf->allocation_end == (uint8_t *)ring_buf->allocation + size));

    /* The allocator pointer itself must not be altered */
    assert(allocator == aws_default_allocator());
}
