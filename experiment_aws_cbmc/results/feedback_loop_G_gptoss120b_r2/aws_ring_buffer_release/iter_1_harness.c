#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_ring_buffer_release_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buffer;
    /* Assume the ring buffer is a valid structure */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    struct aws_byte_buf buf;
    /* Allocate a buffer for the aws_byte_buf */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Optional: assume the buffer belongs to the ring buffer pool */
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 4. Assert postconditions */

    /* The tail atomic pointer must be updated to point just past the released buffer */
    uint8_t *new_tail = AWS_ATOMIC_LOAD_PTR(&ring_buffer.tail);
    assert(new_tail == old_buf.buffer + old_buf.capacity);

    /* The released buffer must be zeroed */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(ring_buffer.allocator == old_ring.allocator);
    assert(ring_buffer.allocation == old_ring.allocation);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);

    /* head atomic pointer must remain unchanged */
    uint8_t *old_head = AWS_ATOMIC_LOAD_PTR(&old_ring.head);
    uint8_t *new_head = AWS_ATOMIC_LOAD_PTR(&ring_buffer.head);
    assert(new_head == old_head);

    /* tail atomic pointer is the only field that may change (already asserted above) */
    /* No other fields of ring_buffer should change */
    /* (allocator, allocation, allocation_end, head already checked) */

    /* 6. Assert validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
