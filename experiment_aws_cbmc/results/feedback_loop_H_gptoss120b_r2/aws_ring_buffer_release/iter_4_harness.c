#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_release_harness(void) {
    uint8_t storage[MAX_BUFFER_SIZE];
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_ring_buffer ring_buffer;
    aws_ring_buffer_init(&ring_buffer, allocator, storage, alloc_size);

    size_t buf_capacity = nondet_size_t();
    __CPROVER_assume(buf_capacity <= alloc_size);
    size_t offset = nondet_size_t();
    __CPROVER_assume(offset + buf_capacity <= alloc_size);

    struct aws_byte_buf buf;
    buf.buffer = storage + offset;
    buf.capacity = buf_capacity;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = allocator;

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    struct aws_ring_buffer old_ring = ring_buffer;
    struct aws_byte_buf old_buf = buf;
    size_t old_head = aws_atomic_load_int(&ring_buffer.head);
    size_t old_tail = aws_atomic_load_int(&ring_buffer.tail);

    aws_ring_buffer_release(&ring_buffer, &buf);

    assert(aws_atomic_load_int(&ring_buffer.tail) == old_tail + old_buf.capacity);
    assert(aws_atomic_load_int(&ring_buffer.head) == old_head);
    assert(ring_buffer.allocator == old_ring.allocator);
    assert(ring_buffer.allocation == old_ring.allocation);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);

    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
