#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    struct aws_ring_buffer ring;
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize ring buffer allocation */
    size_t ring_alloc_size = nondet_size_t();
    __CPROVER_assume(ring_alloc_size > 0);
    __CPROVER_assume(ring_alloc_size <= 1024);
    ring.allocation = malloc(ring_alloc_size);
    __CPROVER_assume(ring.allocation != NULL);
    ring.allocation_end = ring.allocation + ring_alloc_size;

    /* Initialize head and tail indices */
    ring.head.value = nondet_size_t();
    ring.tail.value = nondet_size_t();
    __CPROVER_assume(ring.head.value <= ring_alloc_size);
    __CPROVER_assume(ring.tail.value <= ring_alloc_size);

    ring.allocator = allocator;

    /* Initialize byte buffer */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, 1024));

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

    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
}
