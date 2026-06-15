#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring;
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    /* 4. Assert postconditions – function is pure, no side‑effects */
    /* Unchanged fields of aws_ring_buffer */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(aws_atomic_load_int(&ring.head) == aws_atomic_load_int(&old_ring.head));
    assert(aws_atomic_load_int(&ring.tail) == aws_atomic_load_int(&old_ring.tail));

    /* Unchanged fields of aws_byte_buf */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.owns_buffer == old_buf.owns_buffer);

    /* 5. Validity invariants must still hold */
    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
