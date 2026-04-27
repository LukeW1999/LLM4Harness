#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_init_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator allocator;
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    /* 2. Initialize allocator */
    make_and_initialize_allocator(&allocator, size);

    /* 3. Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old_ring_buf = ring_buf;

    /* 4. Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, &allocator, size);

    /* 5. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocator == &allocator);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
    } else {
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == &allocator);
        assert(ring_buf.allocation_end == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
    }

    /* 6. Assert fields that must NOT change regardless of result */
    assert(ring_buf.allocator == &allocator);

    /* 7. Assert validity invariants always holds */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
