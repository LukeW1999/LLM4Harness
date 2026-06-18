#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness(void) {
    /* 1. Declare inputs */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size = nondet_size_t();

    /* Bound the size to keep the state space finite */
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(size > 0);

    /* 2. Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 3. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Allocation must have succeeded */
        assert(ring_buf.allocation != NULL);
        /* The allocator stored must be the one we passed */
        assert(ring_buf.allocator == allocator);
        /* allocation_end must point exactly size bytes after allocation */
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
        /* The ring buffer must satisfy its validity predicate */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* On failure the allocation is NULL and the structure remains zeroed */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation_end == NULL);
        /* The ring buffer is not valid when initialization fails */
        assert(!aws_ring_buffer_is_valid(&ring_buf));
    }
}
