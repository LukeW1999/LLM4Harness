#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_acquire_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf dest;
    size_t requested_size = nondet_size_t();
    int result;

    // Initialize ring_buf with arbitrary values
    ring_buf.allocator = aws_default_allocator();
    size_t allocation_size = nondet_size_t();
    ring_buf.allocation = bounded_malloc(allocation_size);
    ring_buf.allocation_end = ring_buf.allocation + allocation_size;
    AWS_ATOMIC_INIT_BOOL(ring_buf.head, 0);
    AWS_ATOMIC_INIT_BOOL(ring_buf.tail, 0);

    // Initialize dest with arbitrary values
    dest.buffer = NULL;
    dest.len = 0;
    dest.capacity = 0;
    dest.allocator = aws_default_allocator();

    // Save old state
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_dest = dest;

    // Assume preconditions
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    __CPROVER_assume(requested_size <= (size_t)(ring_buf.allocation_end - ring_buf.allocation));
    __CPROVER_assume(AWS_ATOMIC_LOAD_BOOL(ring_buf.head) <= AWS_ATOMIC_LOAD_BOOL(ring_buf.tail));
    __CPROVER_assume(AWS_ATOMIC_LOAD_BOOL(ring_buf.tail) <= (ring_buf.allocation_end - ring_buf.allocation));

    // Call function under test
    result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    // Assert frame conditions and validity invariants
    if (result == AWS_OP_SUCCESS) {
        assert(AWS_ATOMIC_LOAD_BOOL(ring_buf.head) == AWS_ATOMIC_LOAD_BOOL(old_ring_buf.head) + requested_size);
        assert(AWS_ATOMIC_LOAD_BOOL(ring_buf.tail) == AWS_ATOMIC_LOAD_BOOL(old_ring_buf.tail));
        assert(dest.buffer == old_ring_buf.allocation + AWS_ATOMIC_LOAD_BOOL(old_ring_buf.head));
        assert(dest.len == requested_size);
        assert(dest.capacity >= requested_size);
        assert(dest.allocator == old_dest.allocator);
    } else {
        assert(AWS_ATOMIC_LOAD_BOOL(ring_buf.head) == AWS_ATOMIC_LOAD_BOOL(old_ring_buf.head));
        assert(AWS_ATOMIC_LOAD_BOOL(ring_buf.tail) == AWS_ATOMIC_LOAD_BOOL(old_ring_buf.tail));
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
    }

    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_bounded(&dest, (size_t)(ring_buf.allocation_end - ring_buf.allocation)));
}
