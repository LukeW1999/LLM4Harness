#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_init_harness(void) {
    /* 1. Declare the ring buffer structure */
    struct aws_ring_buffer ring_buf;

    /* 2. Non‑deterministic size bounded by a reasonable maximum */
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    /* 3. Use the default allocator (may nondeterministically fail) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 4. Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 5. Post‑conditions for the success path */
    if (result == AWS_OP_SUCCESS) {
        /* allocation must be non‑NULL and exactly the requested size */
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        /* allocator must be the one we passed in */
        assert(ring_buf.allocator == allocator);

        /* head and tail must both point to the start of the allocation */
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);

        /* the ring buffer must satisfy its validity predicate */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* On failure the allocation failed, so allocation must be NULL */
        assert(ring_buf.allocation == NULL);

        /* allocator, head, tail, and allocation_end remain zeroed */
        assert(ring_buf.allocator == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
        assert(ring_buf.allocation_end == NULL);

        /* The ring buffer is not valid when initialization fails */
        assert(!aws_ring_buffer_is_valid(&ring_buf));
    }
}
