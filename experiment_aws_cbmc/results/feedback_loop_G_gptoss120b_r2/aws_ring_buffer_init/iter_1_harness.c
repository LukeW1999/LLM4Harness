#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>

void aws_ring_buffer_init_harness(void) {
    /* 1. Declare the ring buffer structure */
    struct aws_ring_buffer ring_buf;
    struct aws_ring_buffer old = ring_buf; /* save nondet initial state */

    /* 2. Non‑deterministic inputs, bounded as required */
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size = nondet_size_t();
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* 3. Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 4. Post‑condition checks for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* allocation must be non‑NULL and point to a region of the requested size */
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocation_end == (uint8_t *)ring_buf.allocation + size);

        /* allocator field must be set to the argument */
        assert(ring_buf.allocator == allocator);

        /* head and tail atomic pointers must be initialized to the allocation base */
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
    } else {
        /* on failure the structure remains zeroed */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(ring_buf.allocator == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
    }

    /* 5. Fields that never change (none in this function beyond those above) */

    /* 6. Validity invariant must always hold */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
