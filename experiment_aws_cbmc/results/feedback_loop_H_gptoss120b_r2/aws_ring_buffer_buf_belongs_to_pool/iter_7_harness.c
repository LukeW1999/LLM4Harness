#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 256

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    struct aws_ring_buffer ring;
    struct aws_allocator *allocator = aws_default_allocator();
    ring.allocator = allocator;

    uint8_t ring_storage[MAX_BUFFER_SIZE];
    size_t ring_size = (size_t)nondet_uint64_t();
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);
    __CPROVER_assume((ring_size & (ring_size - 1)) == 0); /* power of two */

    ring.allocation = ring_storage;
    ring.allocation_end = ring_storage + ring_size;
    ring.size = ring_size;
    ring.size_mask = ring_size - 1;

    aws_atomic_store_int(&ring.head, 0);
    aws_atomic_store_int(&ring.tail, 0);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.size == old_ring.size);
    assert(ring.size_mask == old_ring.size_mask);
    assert(aws_atomic_load_int(&ring.head) == aws_atomic_load_int(&old_ring.head));
    assert(aws_atomic_load_int(&ring.tail) == aws_atomic_load_int(&old_ring.tail));

    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    if (result) {
        assert(buf.buffer >= ring.allocation);
        assert(buf.buffer < ring.allocation_end);
    } else {
        assert(!(buf.buffer >= ring.allocation && buf.buffer < ring.allocation_end));
    }

    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
