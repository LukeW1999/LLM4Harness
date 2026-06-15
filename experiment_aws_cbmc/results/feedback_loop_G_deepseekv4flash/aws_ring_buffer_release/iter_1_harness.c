#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness() {
    /* parameters */
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* preconditions */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    __CPROVER_assume(buf.capacity < MAX_BUFFER_SIZE);
    __CPROVER_assume(buf.buffer != NULL); /* avoid null pointer in arithmetic */
    __CPROVER_assume((uintptr_t)buf.buffer + buf.capacity >= (uintptr_t)buf.buffer); /* no overflow */

    /* save old ring buffer for unchanged field checks */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    uint8_t *old_tail = (uint8_t *)aws_atomic_load_ptr(&old_ring_buffer.tail);

    /* call function */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* postconditions */
    /* 1. ring buffer remains valid */
    assert(aws_ring_buffer_is_valid(&ring_buffer));

    /* 2. buf is zeroed */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* 3. unchanged fields of ring_buffer */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);

    /* head (atomic) is not changed */
    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&old_ring_buffer.head);
    assert(new_head == old_head);

    /* 4. tail is updated to the end of the released buffer */
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    uint8_t *expected_tail = buf.buffer + buf.capacity;
    assert(new_tail == expected_tail);

    /* 5. ensure the new tail points within the ring buffer */
    assert(aws_ring_buffer_check_atomic_ptr(&ring_buffer, new_tail));
}
