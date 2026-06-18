#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 256

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    struct aws_ring_buffer ring;
    struct aws_allocator *allocator = aws_default_allocator();
    ring.allocator = allocator;

    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);
    uint8_t allocation[MAX_BUFFER_SIZE];
    ring.allocation = allocation;
    ring.allocation_end = allocation + alloc_size;

    ring.head = (struct aws_atomic_var){ .value = nondet_uint64_t() };
    ring.tail = (struct aws_atomic_var){ .value = nondet_uint64_t() };

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    /* set allocator and length to satisfy aws_byte_buf_is_valid */
    buf.allocator = allocator;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    /* Verify that the ring buffer and byte buffer remain unchanged */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.head.value == old_ring.head.value);
    assert(ring.tail.value == old_ring.tail.value);

    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* The result should reflect whether the byte buffer lies within the ring's allocation */
    bool in_range = (buf.buffer >= ring.allocation) && (buf.buffer < ring.allocation_end);
    assert(result == in_range);

    /* Re‑establish invariants */
    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
