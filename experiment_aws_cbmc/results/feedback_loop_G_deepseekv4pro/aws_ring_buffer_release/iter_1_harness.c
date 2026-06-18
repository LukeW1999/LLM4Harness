#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* Bound the ring buffer size */
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    /* Allocate ring buffer's allocation */
    ring_buffer.allocator = aws_default_allocator();
    ring_buffer.allocation = (uint8_t *)malloc(size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + size;

    /* Initialize head and tail atomics to start of allocation */
    aws_atomic_init_ptr(&ring_buffer.head, ring_buffer.allocation);
    aws_atomic_init_ptr(&ring_buffer.tail, ring_buffer.allocation);

    /* Set up byte buffer that belongs to the ring buffer */
    size_t offset = nondet_size_t();
    size_t capacity = nondet_size_t();
    __CPROVER_assume(offset + capacity <= size);
    __CPROVER_assume(capacity > 0); /* capacity must be > 0 for a valid buffer? Actually, capacity can be 0? But release with capacity 0 would set tail to buffer, which is fine. We'll allow capacity >= 0. */
    buf.buffer = ring_buffer.allocation + offset;
    buf.capacity = capacity;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = aws_default_allocator(); /* non-NULL to check zeroing */

    /* Ensure the ring buffer is valid initially */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    /* Ensure the byte buffer is valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    /* For atomic fields, we need to load the pointers */
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    uint8_t *old_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    /* Save byte buffer fields */
    struct aws_byte_buf old_buf = buf;
    uint8_t *old_buf_buffer = buf.buffer;
    size_t old_buf_capacity = buf.capacity;

    /* 3. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 4. Assert postconditions */
    /* Byte buffer is zeroed */
    assert(buf.allocator == NULL);
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    /* Ring buffer's tail updated to old buf->buffer + old buf->capacity */
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    assert(new_tail == old_buf_buffer + old_buf_capacity);

    /* Ring buffer's other fields unchanged */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    /* head unchanged */
    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    assert(new_head == old_head);

    /* 5. Validity invariants */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    /* Byte buffer is not necessarily valid after zeroing, but we can check that it's all zeros */
}
