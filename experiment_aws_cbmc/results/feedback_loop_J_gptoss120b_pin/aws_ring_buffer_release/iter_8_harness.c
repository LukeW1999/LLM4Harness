#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring;
    struct aws_byte_buf buf;

    struct aws_allocator *allocator = aws_default_allocator();

    size_t ring_size = nondet_uint64_t();
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);

    aws_ring_buffer_init(&ring, allocator, ring_size);

    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    aws_ring_buffer_release(&ring, &buf);

    assert(ring.allocation == NULL);
    assert(ring.allocation_end == NULL);
    assert(aws_atomic_load_int(&ring.head) == 0);
    assert(aws_atomic_load_int(&ring.tail) == 0);
    assert(ring.size == 0);
    assert(ring.allocator == old_ring.allocator);

    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    assert(aws_ring_buffer_is_valid(&ring));
}
