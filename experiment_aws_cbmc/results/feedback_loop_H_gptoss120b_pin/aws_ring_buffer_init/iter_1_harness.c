#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness() {
    /* 1. Declare data structures */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size = nondet_size_t();

    /* Bound the size to keep the state space finite */
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* 2. Save old state before the call */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 4. Post‑condition checks for both success and failure paths */
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

    /* 5. Fields that must not change regardless of result are already covered above */

    /* 6. The ring buffer must always satisfy its validity invariant */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
