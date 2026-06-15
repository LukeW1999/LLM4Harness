#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* 1. Declare data structures */
    struct aws_ring_buffer ring;
    struct aws_byte_buf buf;

    /* 2. Set up allocator */
    ring.allocator = aws_default_allocator();

    /* 3. Allocate ring buffer memory region */
    size_t ring_capacity = nondet_size_t();
    __CPROVER_assume(ring_capacity > 0 && ring_capacity <= MAX_BUFFER_SIZE);
    uint8_t *ring_mem = (uint8_t *)aws_mem_acquire(ring.allocator, ring_capacity);
    __CPROVER_assume(ring_mem != NULL);
    ring.allocation = ring_mem;
    ring.allocation_end = ring_mem + ring_capacity;
    ring.size = ring_capacity;

    /* 4. Initialize head and tail within the allocation */
    ring.head = (size_t)nondet_uintptr_t() % ring_capacity;
    ring.tail = (size_t)nondet_uintptr_t() % ring_capacity;

    /* 5. Ensure the byte buffer has an allocated backing buffer */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 6. Nondeterministically decide whether buf.buffer lies inside the ring allocation */
    bool inside = nondet_bool();
    if (inside) {
        size_t offset = (size_t)nondet_uintptr_t() % ring_capacity;
        buf.buffer = ring.allocation + offset;
        buf.capacity = ring_capacity - offset;
        buf.len = (size_t)nondet_uintptr_t() % (buf.capacity + 1);
    } else {
        uint8_t *outside = (uint8_t *)aws_mem_acquire(ring.allocator, 1);
        __CPROVER_assume(outside != NULL);
        buf.buffer = outside;
        buf.capacity = 1;
        buf.len = 0;
        __CPROVER_assume(buf.buffer < ring.allocation || buf.buffer >= ring.allocation_end);
    }

    /* 7. Assume ring buffer invariant holds */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* 8. Save old state */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 9. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    /* 10. Assert that the function does not modify inputs */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.head == old_ring.head);
    assert(ring.tail == old_ring.tail);
    assert(ring.size == old_ring.size);

    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* 11. Invariants must still hold */
    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 12. Logical post‑condition */
    if (result) {
        assert(buf.buffer >= ring.allocation);
        assert(buf.buffer < ring.allocation_end);
    }
}
