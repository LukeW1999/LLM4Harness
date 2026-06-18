#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    struct aws_ring_buffer ring;
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();

    uint8_t raw_allocation[1024];
    size_t max_alloc = nondet_uint64_t();
    __CPROVER_assume(max_alloc > 0);
    __CPROVER_assume(max_alloc <= sizeof(raw_allocation));

    size_t size = nondet_uint64_t();
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size <= max_alloc);
    __CPROVER_assume((size & (size - 1)) == 0);

    ring.allocation = raw_allocation;
    ring.allocation_end = raw_allocation + size;
    ring.size = size;
    ring.mask = size - 1;

    size_t head_val = nondet_uint64_t();
    size_t tail_val = nondet_uint64_t();
    __CPROVER_assume(head_val < size);
    __CPROVER_assume(tail_val < size);
    aws_atomic_init_int(&ring.head, (int)head_val);
    aws_atomic_init_int(&ring.tail, (int)tail_val);

    ring.allocator = allocator;

    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, 1024));
    buf.allocator = allocator;
    size_t len = nondet_uint64_t();
    __CPROVER_assume(len <= buf.capacity);
    buf.len = len;

    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);
    (void)result;

    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(aws_atomic_load_int(&ring.head) == aws_atomic_load_int(&old_ring.head));
    assert(aws_atomic_load_int(&ring.tail) == aws_atomic_load_int(&old_ring.tail));
    assert(ring.size == old_ring.size);
    assert(ring.mask == old_ring.mask);

    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
}
