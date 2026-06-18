#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_release_harness() {
    /* 1. Allocate a backing buffer for the ring buffer on the stack */
    uint8_t backing[MAX_BUFFER_SIZE];
    struct aws_ring_buffer ring = {0};
    size_t alloc_size = nondet_uint64_t();
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_BUFFER_SIZE);
    ring.allocation = backing;
    ring.allocation_end = backing + alloc_size;
    struct aws_allocator *allocator = aws_default_allocator();
    ring.allocator = allocator;

    /* Assume the ring buffer is valid after this initialization */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* 2. Declare and bound the byte buffer that will be released */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    size_t buf_cap = nondet_uint64_t();
    __CPROVER_assume(buf_cap <= MAX_BUFFER_SIZE);
    buf.capacity = buf_cap;
    buf.len = nondet_uint64_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = allocator;

    /* Ensure the buffer lies within the ring buffer's allocation */
    size_t offset = nondet_uint64_t();
    __CPROVER_assume(offset + buf.capacity <= alloc_size);
    buf.buffer = ring.allocation + offset;

    /* 3. Save old state */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 4. Call the function under test */
    aws_ring_buffer_release(&ring, &buf);

    /* 5. Post‑conditions */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* Ring buffer fields that must not change */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    /* Tail may change; no concrete assertion */

    /* 6. Ring buffer must remain valid */
    assert(aws_ring_buffer_is_valid(&ring));
}
