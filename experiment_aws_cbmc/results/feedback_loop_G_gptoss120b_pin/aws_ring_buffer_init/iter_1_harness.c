#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness() {
    /* 1. Declare inputs */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size = nondet_size_t();

    /* Bound the size to avoid unbounded allocation */
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* Save old state before the call */
    struct aws_ring_buffer old = ring_buf;

    /* 2. Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 3. Postconditions for success */
    if (result == AWS_OP_SUCCESS) {
        /* allocation must be non‑NULL */
        assert(ring_buf.allocation != NULL);
        /* allocator must be the one we passed */
        assert(ring_buf.allocator == allocator);
        /* allocation_end must be allocation + size */
        assert(ring_buf.allocation_end == (uint8_t *)ring_buf.allocation + size);
        /* head and tail atomic pointers must point to the allocation */
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
        /* The ring buffer must satisfy its validity predicate */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* 4. Postconditions for failure */
        /* The function zeroes the structure before attempting allocation */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
        /* No validity guarantee on failure */
    }

    /* 5. Fields that must not change regardless of result */
    /* The input allocator pointer itself is not stored elsewhere, so nothing else to check */
    (void)old; /* suppress unused variable warning */
}
