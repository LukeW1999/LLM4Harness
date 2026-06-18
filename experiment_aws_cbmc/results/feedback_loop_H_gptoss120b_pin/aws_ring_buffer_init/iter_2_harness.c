#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_ring_buffer_init_harness(void) {
    /* Allocate and initialize the ring buffer structure */
    struct aws_ring_buffer ring_buf;
    /* Ensure the structure starts in a known state */
    memset(&ring_buf, 0, sizeof(ring_buf));

    /* Use the default allocator (may nondeterministically fail) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Nondeterministic size, bounded to keep the state space finite */
    size_t size = nondet_size_t();
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* Post‑condition checks for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Allocation must have succeeded */
        assert(ring_buf.allocation != NULL);
        /* Allocator field must be set to the provided allocator */
        assert(ring_buf.allocator == allocator);
        /* Head and tail atomic pointers must point to the allocation */
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
        /* allocation_end must be allocation + size */
        assert(ring_buf.allocation_end == (uint8_t *)ring_buf.allocation + size);
    } else {
        /* Allocation failed – all fields must remain NULL/zero */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
        assert(ring_buf.allocation_end == NULL);
    }

    /* The ring buffer must always satisfy its validity invariant */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
