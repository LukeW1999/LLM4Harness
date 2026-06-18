#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness(void) {
    /* Allocate the ring buffer structure on the heap */
    struct aws_ring_buffer *ring_buf = malloc(sizeof(struct aws_ring_buffer));
    /*
     * Since aws_ring_buffer_init requires a non‑NULL ring_buf, we assume
     * that the allocation succeeds.  CBMC tracks both possibilities for
     * malloc, but the path where it returns NULL would violate the
     * precondition and is therefore excluded.
     */
    __CPROVER_assume(ring_buf != NULL);

    /* Use the default (non‑failing) allocator.  Internally this calls
     * malloc, which CBMC models as nondeterministic – it may return NULL,
     * simulating an allocation failure.
     */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Choose an arbitrary but bounded buffer size */
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= 4096);

    /* Call the function under verification */
    int result = aws_ring_buffer_init(ring_buf, allocator, size);

    /* ------------------------------------------------------------------ */
    /*  Post‑condition checks                                             */
    /* ------------------------------------------------------------------ */

    /* The return value must be either success or failure */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* Successful initialisation: the allocation cannot be NULL */
        assert(ring_buf->allocation != NULL);
        /* The allocator pointer must be preserved */
        assert(ring_buf->allocator == allocator);
        /* The end pointer must be exactly size bytes after the start */
        assert(ring_buf->allocation_end == ring_buf->allocation + size);
        /* The whole structure must pass the validity check */
        assert(aws_ring_buffer_is_valid(ring_buf));
    } else {
        /* On failure the struct must be fully zeroed */
        assert(ring_buf->allocation == NULL);
        assert(ring_buf->allocator == NULL);
        assert(ring_buf->allocation_end == NULL);
    }

    /* Clean up only when the initialisation succeeded */
    if (result == AWS_OP_SUCCESS) {
        aws_ring_buffer_clean_up(ring_buf);
    }

    /* Release the ring‑buffer structure itself */
    free(ring_buf);
}

/*
 * Entry point required by CBMC: it calls the harness so that all
 * assertions become reachable during the verification run.
 */
void aws_ring_buffer_init_harness(void) {
    aws_ring_buffer_init_harness();
    return 0;
}
