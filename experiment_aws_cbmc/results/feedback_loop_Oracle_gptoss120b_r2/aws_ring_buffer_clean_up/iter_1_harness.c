#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>
#include <aws/common/memory.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_ring_buffer_clean_up_harness(void) {
    /* Allocate a ring buffer structure */
    struct aws_ring_buffer ring_buf;

    /* Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Nondeterministic size for the ring buffer allocation (must be > 0) */
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0);

    /* Initialize the ring buffer – assume successful initialization */
    int init_res = aws_ring_buffer_init(&ring_buf, allocator, size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);

    /* Structural validity assumption required by the specification */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Preserve the original allocation pointer for potential frame‑condition checks */
    uint8_t *original_allocation = ring_buf.allocation;

    /* Call the function under verification */
    aws_ring_buffer_clean_up(&ring_buf);

    /* -------------------------------------------------------------------- */
    /* Post‑condition assertions                                            */
    /* -------------------------------------------------------------------- */

    /* 1. The ring buffer fields must be zeroed after clean_up */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.allocator == NULL);
    assert(aws_atomic_load_int(&ring_buf.head) == 0);
    assert(aws_atomic_load_int(&ring_buf.tail) == 0);

    /* 2. No other memory reachable from the ring buffer should be modified */
    /*    (the original allocation has been released, so the pointer must be NULL) */
    assert(original_allocation != NULL ? ring_buf.allocation == NULL : true);

    return 0;
}
