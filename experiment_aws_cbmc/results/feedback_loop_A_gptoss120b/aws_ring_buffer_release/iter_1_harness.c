#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_ring_buffer_release_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* Assume the ring buffer is a valid, bounded instance */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Assume the byte buffer is bounded and has an allocated buffer member */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old_ring = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 4. Assert postconditions for BOTH success and failure paths
       (aws_ring_buffer_release returns void, so only one path) */

    /* 4a. Changed fields */
    /* The tail atomic variable must now point to the end of the released buffer */
    assert(((uint8_t *)ring_buffer.tail.value) == (old_buf.buffer + old_buf.capacity));

    /* The byte buffer must be zeroed */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);
    /* If aws_byte_buf has other fields, they are also expected to be zero */
    /* (e.g., buf.len, buf.capacity already covered) */

    /* 4b. Unchanged fields of the ring buffer */
    assert(ring_buffer.allocator == old_ring.allocator);
    assert(ring_buffer.allocation == old_ring.allocation);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);
    assert(((uint8_t *)ring_buffer.head.value) == ((uint8_t *)old_ring.head.value));

    /* 5. Assert validity invariants */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    /* The zeroed byte buffer is still a valid (empty) buffer */
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
