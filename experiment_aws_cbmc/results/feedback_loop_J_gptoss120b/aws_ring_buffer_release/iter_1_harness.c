#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buffer;
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    struct aws_byte_buf buf;
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

    /* Changed field: the tail atomic pointer must now point to the end of the released buffer */
    assert(aws_ring_buffer_check_atomic_ptr(&ring_buffer,
                                            old_buf.buffer + old_buf.capacity));

    /* Changed fields of the byte buffer: it must be zeroed */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* Unchanged fields of the ring buffer (except tail) */
    assert(ring_buffer.allocator == old_ring.allocator);
    assert(ring_buffer.allocation == old_ring.allocation);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);
    assert(ring_buffer.head == old_ring.head);

    /* 5. Validity invariants */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
