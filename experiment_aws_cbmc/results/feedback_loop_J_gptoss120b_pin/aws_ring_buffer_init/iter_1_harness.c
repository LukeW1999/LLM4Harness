#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>

void aws_ring_buffer_init_harness(void) {
    /* 1. Declare the ring buffer and related inputs */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size = nondet_size_t();

    /* Bound the size to keep the state space finite */
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* 2. Save old state before the call (for potential immutability checks) */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 4. Postcondition checks for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Allocation must be non‑NULL and correctly sized */
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        /* Allocator must be the one we passed in */
        assert(ring_buf.allocator == allocator);

        /* Head and tail atomic pointers must point to the start of the allocation */
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);

        /* The ring buffer must satisfy its validity predicate */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* On failure the ring buffer should be left in a zeroed state */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
        assert(ring_buf.allocation_end == NULL);
    }

    /* 5. Fields that must remain unchanged regardless of the result */
    /* (All fields are either set on success or zeroed on failure, so no additional unchanged‑field asserts are needed) */
}
