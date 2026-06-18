#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness(void) {
    /* 1. Declare structures */
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* Allocate ring buffer memory */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);
    ring_buffer.allocation = malloc(alloc_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + alloc_size;
    ring_buffer.allocator = aws_default_allocator();

    /* Initialize atomic head and tail to some valid location inside allocation */
    uint8_t *head_ptr = ring_buffer.allocation;
    uint8_t *tail_ptr = ring_buffer.allocation;
    ring_buffer.head.ptr = (void *)head_ptr;
    ring_buffer.tail.ptr = (void *)tail_ptr;

    /* 2. Set up a byte buffer that belongs to the ring buffer pool */
    size_t buf_capacity = nondet_size_t();
    __CPROVER_assume(buf_capacity <= alloc_size);
    size_t offset = nondet_size_t();
    __CPROVER_assume(offset + buf_capacity <= alloc_size);
    buf.buffer = ring_buffer.allocation + offset;
    buf.capacity = buf_capacity;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = aws_default_allocator();

    /* 3. Assume the ring buffer is valid before the call */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* 4. Save old state */
    struct aws_ring_buffer old_ring = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* 5. Call the function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 6. Post‑conditions */

    /* Tail pointer must be updated to point just past the released region */
    assert(ring_buffer.tail.ptr == (void *)(old_buf.buffer + old_buf.capacity));

    /* All other ring buffer fields must remain unchanged */
    assert(ring_buffer.head.ptr == old_ring.head.ptr);
    assert(ring_buffer.allocator == old_ring.allocator);
    assert(ring_buffer.allocation == old_ring.allocation);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);

    /* The released byte buffer must be zeroed */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* Validity invariants must still hold */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
