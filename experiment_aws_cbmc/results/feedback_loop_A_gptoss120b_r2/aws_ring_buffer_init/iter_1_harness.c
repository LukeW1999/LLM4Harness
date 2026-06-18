#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Bounding macro – adjust as needed for the proof environment */
#ifndef MAX_BUFFER_SIZE
#   define MAX_BUFFER_SIZE 1024
#endif

void aws_ring_buffer_init_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    /* Assume the ring buffer is initially in a valid state (if any) */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Use the default allocator as required */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Nondeterministic size bounded by MAX_BUFFER_SIZE */
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Allocation must be non‑NULL and exactly the requested size */
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocation_end == (uint8_t *)ring_buf.allocation + size);

        /* Allocator must be the one we passed in */
        assert(ring_buf.allocator == allocator);

        /* Head and tail atomic pointers must both point to the start of the allocation */
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
    } else {
        /* On failure the structure should be all zeros (as AWS_ZERO_STRUCT was applied) */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(ring_buf.allocator == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* No other fields exist in aws_ring_buffer besides those already examined,
       so no additional unchanged‑field asserts are required. */

    /* 6. Assert validity invariant always holds on success */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }
}
