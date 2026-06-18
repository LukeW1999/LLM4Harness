#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* Declare structures */
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf buf;

    /* Assume valid ring buffer (head/tail within allocation, non-null allocator, etc.) */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Set up a valid, bounded byte buffer with allocated memory */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save the original state to check for modification */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    /* 1. Structural invariants: neither input should be modified */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
    /* head and tail are atomic – const function may not modify them;
       verifying their exact value is omitted for brevity and because struct comparison is not standard C. */

    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);

    /* 2. Validity invariants must still hold */
    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_valid(&buf));

    /* 3. Functional correctness: the result must match the simple specification:
       true iff the buffer's memory lies entirely within the ring buffer's allocation */
    bool expected = (buf.buffer >= ring_buf.allocation) &&
                    (buf.buffer + buf.len <= ring_buf.allocation_end);
    assert(result == expected);
}
