#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* Declare structures */
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf buf;

    /* Ensure ring buffer is valid and bounded */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    __CPROVER_assume(ring_buf.allocation_end - ring_buf.allocation <= MAX_BUFFER_SIZE);

    /* Ensure byte buf has allocated buffer member and is valid */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);

    /* Save old state for immutability check */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    /* Postconditions */

    /* 1. The function must not modify the ring buffer (const) */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
    /* head and tail are atomic; load and compare */
    assert((uint8_t *)aws_atomic_load_ptr(&ring_buf.head) ==
           (uint8_t *)aws_atomic_load_ptr(&old_ring_buf.head));
    assert((uint8_t *)aws_atomic_load_ptr(&ring_buf.tail) ==
           (uint8_t *)aws_atomic_load_ptr(&old_ring_buf.tail));

    /* 2. The function must not modify the byte buf (const) */
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);

    /* 3. The return value must match the expected belonging condition:
     *    true iff buf.buffer lies within [ring_buf.allocation, ring_buf.allocation_end)
     *    and the entire buffer (capacity) fits within that range.
     */
    bool expected = (buf.buffer >= ring_buf.allocation) &&
                    (buf.buffer + buf.capacity <= ring_buf.allocation_end);
    assert(result == expected);

    /* 4. Validity invariants remain */
    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_valid(&buf));
}
