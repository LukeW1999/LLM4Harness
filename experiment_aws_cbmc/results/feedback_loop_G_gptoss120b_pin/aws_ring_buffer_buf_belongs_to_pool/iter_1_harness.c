#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring;
    struct aws_byte_buf buf;

    /* allocator */
    ring.allocator = aws_default_allocator();

    /* nondet allocation for the ring buffer */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);
    ring.allocation = (uint8_t *)malloc(alloc_size);
    __CPROVER_assume(ring.allocation != NULL);
    ring.allocation_end = ring.allocation + alloc_size;

    /* initialize atomic vars (nondet but bounded) */
    ring.head = (struct aws_atomic_var){0};
    ring.tail = (struct aws_atomic_var){0};

    /* ensure byte buffer has allocated buffer member and is bounded */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* assume the ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* The function does not modify any state; it only reports membership. */
    /* Unchanged fields */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.head == old_ring.head);
    assert(ring.tail == old_ring.tail);

    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* Result matches membership condition */
    bool in_range = (buf.buffer >= ring.allocation) && (buf.buffer < ring.allocation_end);
    assert(result == in_range);

    /* 5. Assert validity invariants always hold */
    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
