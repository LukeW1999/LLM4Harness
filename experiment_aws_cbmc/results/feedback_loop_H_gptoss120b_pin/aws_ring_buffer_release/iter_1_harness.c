#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* Allocate ring buffer memory and bound its size */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);
    ring_buffer.allocation = (uint8_t *)malloc(ring_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + ring_size;
    ring_buffer.allocator = aws_default_allocator();

    /* Initialise atomic head and tail (values are not important for the test) */
    AWS_ATOMIC_STORE_PTR(&ring_buffer.head, ring_buffer.allocation);
    AWS_ATOMIC_STORE_PTR(&ring_buffer.tail, ring_buffer.allocation);

    /* Set up a byte buffer that points inside the ring buffer allocation */
    size_t offset = nondet_size_t();
    __CPROVER_assume(offset <= ring_size);
    size_t cap = nondet_size_t();
    __CPROVER_assume(cap <= ring_size - offset);
    buf.buffer = ring_buffer.allocation + offset;
    buf.capacity = cap;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= cap);
    buf.allocator = aws_default_allocator();

    /* Assume the ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 4. Assert postconditions */

    /* Tail pointer must be updated to point just past the released region */
    uint8_t *new_tail = AWS_ATOMIC_LOAD_PTR(&ring_buffer.tail);
    assert(new_tail == old_buf.buffer + old_buf.capacity);

    /* The byte buffer must be zeroed */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(ring_buffer.allocator == old_ring.allocator);
    assert(ring_buffer.allocation == old_ring.allocation);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);

    /* Head pointer must remain unchanged */
    uint8_t *head_ptr = AWS_ATOMIC_LOAD_PTR(&ring_buffer.head);
    uint8_t *old_head = AWS_ATOMIC_LOAD_PTR(&old_ring.head);
    assert(head_ptr == old_head);

    /* 6. Assert validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
