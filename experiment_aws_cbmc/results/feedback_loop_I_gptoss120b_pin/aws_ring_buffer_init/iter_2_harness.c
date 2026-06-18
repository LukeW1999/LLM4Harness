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
#include <string.h>

void aws_ring_buffer_init_harness() {
    /* 1. Declare and zero‑initialize the ring buffer */
    struct aws_ring_buffer ring_buf;
    memset(&ring_buf, 0, sizeof(ring_buf));

    /* 2. Non‑deterministic size, bounded (including zero to hit failure path) */
    size_t size = nondet_size_t();
    __CPROVER_assume(size <= 1024);   /* arbitrary bound */

    /* 3. Use the default allocator (must be non‑NULL) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 4. Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* allocation must be non‑NULL */
        assert(ring_buf.allocation != NULL);
        /* allocator field must be set to the argument */
        assert(ring_buf.allocator == allocator);
        /* head and tail atomic pointers must point to the allocation */
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
        /* allocation_end must be allocation + size */
        assert(ring_buf.allocation_end == (uint8_t *)ring_buf.allocation + size);
    } else {
        /* on failure the struct remains zeroed */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
    }

    /* 6. Validity invariant must always hold */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
