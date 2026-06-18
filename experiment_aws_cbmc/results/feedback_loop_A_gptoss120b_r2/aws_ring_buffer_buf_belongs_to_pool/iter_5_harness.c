#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    struct aws_ring_buffer ring;
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Allocate raw memory for the ring buffer */
    size_t max_alloc = nondet_uint64_t();
    __CPROVER_assume(max_alloc > 0);
    __CPROVER_assume(max_alloc <= 1024);
    ring.allocation = (uint8_t *)malloc(max_alloc);
    __CPROVER_assume(ring.allocation != NULL);

    /* Choose a size that is a power of two and fits in the allocated memory */
    size_t size = nondet_uint64_t();
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size <= max_alloc);
    __CPROVER_assume((size & (size - 1)) == 0); /* power of two */
    ring.size = size;
    ring.mask = size - 1;
    ring.allocation_end = ring.allocation + size;

    /* Initialize head and tail indices within bounds */
    ring.head.value = nondet_uint64_t();
    ring.tail.value = nondet_uint64_t();
    __CPROVER_assume(ring.head.value < size);
    __CPROVER_assume(ring.tail.value < size);

    ring.allocator = allocator;

    /* Initialize a valid byte buffer */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, 1024));

    /* Set allocator and length to satisfy aws_byte_buf_is_valid */
    buf.allocator = allocator;
    size_t len = nondet_uint64_t();
    __CPROVER_assume(len <= buf.capacity);
    buf.len = len;

    /* Save old state */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);
    (void)result;

    /* Verify no side‑effects */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.head.value == old_ring.head.value);
    assert(ring.tail.value == old_ring.tail.value);
    assert(ring.size == old_ring.size);
    assert(ring.mask == old_ring.mask);

    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
}
