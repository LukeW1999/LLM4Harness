#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness(void) {
    /* 1. Declare the ring buffer and nondeterministic size */
    struct aws_ring_buffer ring_buf;
    size_t size = nondet_size_t();

    /* bound the size to keep the state space finite */
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* use the default allocator as required */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Save old state before the call (for failure path checks) */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 4. Post‑condition checks for both success and error paths */
    if (result == AWS_OP_SUCCESS) {
        /* allocation must be non‑NULL */
        assert(ring_buf.allocation != NULL);
        /* allocator must be the one we passed */
        assert(ring_buf.allocator == allocator);
        /* head and tail must point to the start of the allocation */
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
        /* allocation_end must be exactly allocation + size */
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
    } else {
        /* on failure the ring buffer remains empty / zeroed */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
        assert(ring_buf.allocation_end == NULL);
        /* the structure should be unchanged from its pre‑call state (which is all zeroes) */
        assert(ring_buf.allocation == old.allocation);
        assert(ring_buf.allocator == old.allocator);
        assert(aws_atomic_load_ptr(&ring_buf.head) == aws_atomic_load_ptr(&old.head));
        assert(aws_atomic_load_ptr(&ring_buf.tail) == aws_atomic_load_ptr(&old.tail));
        assert(ring_buf.allocation_end == old.allocation_end);
    }

    /* 5. Validity invariant: the ring buffer is valid iff the call succeeded */
    assert(aws_ring_buffer_is_valid(&ring_buf) == (result == AWS_OP_SUCCESS));
}
