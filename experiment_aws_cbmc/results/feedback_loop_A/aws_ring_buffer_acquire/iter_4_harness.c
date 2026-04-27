#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_acquire_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size = nondet_size_t();
    size_t requested_size = nondet_size_t();
    struct aws_byte_buf dest;

    /* 1. Declare and bound data structures */
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(size > requested_size);
    ring_buf.allocator = allocator;
    ring_buf.allocation = (uint8_t *)malloc(size);
    ring_buf.allocation_end = ring_buf.allocation + size;
    ring_buf.head = (struct aws_atomic_var){.var = 0};
    ring_buf.tail = (struct aws_atomic_var){.var = 0};
    __CPROVER_assume(ring_buf.allocation != NULL);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old_ring_buf = ring_buf;

    /* 3. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.buffer == (uint8_t *)ring_buf.allocation + old_ring_buf.head.var);
        assert(dest.len == requested_size);
        assert(ring_buf.head.var == old_ring_buf.head.var + requested_size);
        assert(ring_buf.tail.var == old_ring_buf.tail.var);
    } else {
        assert(ring_buf.head.var == old_ring_buf.head.var);
        assert(ring_buf.tail.var == old_ring_buf.tail.var);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);

    /* 6. Assert validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
