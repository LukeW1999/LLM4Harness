#include <stddef.h>
#include <stdint.h>
#include "aws/common/ring_buffer.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_ring_buffer_release_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring;
    struct aws_byte_buf buf;

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    aws_ring_buffer_release(&ring, &buf);

    /* 4. Assert postconditions */

    /* buf must be zeroed */
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.buffer == NULL);
    assert(buf.allocator == NULL);

    /* ring fields that must not change */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.head == old_ring.head);

    /* tail must be updated to point just past the released buffer */
    assert(aws_ring_buffer_check_atomic_ptr(&ring,
                                            old_buf.buffer + old_buf.capacity));

    /* 5. Validity invariants */
    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
