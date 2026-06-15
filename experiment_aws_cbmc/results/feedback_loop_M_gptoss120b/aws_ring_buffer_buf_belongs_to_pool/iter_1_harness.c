#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Helper to initialize an aws_ring_buffer with bounded allocation */
static void initialize_ring_buffer(struct aws_ring_buffer *ring) {
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    ring->allocator = aws_default_allocator();
    ring->allocation = malloc(size);
    __CPROVER_assume(ring->allocation != NULL);
    ring->allocation_end = ring->allocation + size;

    /* Initialize atomic head and tail to zero (single‑threaded use case) */
    ring->head = (struct aws_atomic_var){0};
    ring->tail = (struct aws_atomic_var){0};

    /* Assume the ring buffer satisfies its validity predicate */
    __CPROVER_assume(aws_ring_buffer_is_valid(ring));
}

/* Harness for aws_ring_buffer_buf_belongs_to_pool */
void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring;
    initialize_ring_buffer(&ring);

    /* 2. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 3. Save old state for immutability checks */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 4. Call the function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    /* 5. Post‑condition: result reflects membership in the ring buffer's allocation */
    if (result) {
        /* The buffer's memory must lie within the ring's allocation range */
        assert(buf.buffer >= ring.allocation);
        assert(buf.buffer + buf.capacity <= ring.allocation_end);
    } else {
        /* If not belonging, then at least one of the above bounds is violated */
        assert(!(buf.buffer >= ring.allocation && buf.buffer + buf.capacity <= ring.allocation_end));
    }

    /* 6. Unchanged fields – ring buffer */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    /* head and tail are atomic; their logical values must remain unchanged */
    assert(ring.head == old_ring.head);
    assert(ring.tail == old_ring.tail);

    /* 7. Unchanged fields – byte buffer */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* 8. Validity invariants */
    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
