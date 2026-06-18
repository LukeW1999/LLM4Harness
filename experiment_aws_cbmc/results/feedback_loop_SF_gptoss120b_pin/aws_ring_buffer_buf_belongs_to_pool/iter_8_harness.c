#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Symbolic inputs */
    ensure_ring_buffer_is_valid(&ring_buffer, allocator);
    ensure_byte_buf_is_valid(&buf, allocator);

    /* Preconditions required by the function */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Additional assumptions matching the function's expectations */
    __CPROVER_assume(ring_buffer.allocation != NULL);
    __CPROVER_assume(ring_buffer.allocation_end != NULL);
    __CPROVER_assume(ring_buffer.allocation_end > ring_buffer.allocation);
    __CPROVER_assume(
        buf.buffer == NULL ||
        (buf.buffer >= ring_buffer.allocation &&
         buf.buffer < ring_buffer.allocation_end &&
         (size_t)(buf.buffer - ring_buffer.allocation) + buf.capacity <=
             (size_t)(ring_buffer.allocation_end - ring_buffer.allocation)));

    /* Snapshot of inputs */
    const uint8_t *orig_buf_ptr = buf.buffer;
    size_t orig_buf_len = buf.len;
    const uint8_t *orig_ring_alloc = ring_buffer.allocation;
    const uint8_t *orig_ring_alloc_end = ring_buffer.allocation_end;

    /* Compute expected result before the call (in case the function mutates buf) */
    bool belongs = false;
    if (orig_buf_ptr != NULL) {
        if (orig_buf_ptr >= ring_buffer.allocation && orig_buf_ptr < ring_buffer.allocation_end) {
            size_t offset = orig_buf_ptr - ring_buffer.allocation;
            if (offset + buf.capacity <= (size_t)(ring_buffer.allocation_end - ring_buffer.allocation)) {
                belongs = true;
            }
        }
    }

    /* Call under verification */
    bool rval = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* Frame conditions: inputs must be unchanged */
    assert(buf.buffer == orig_buf_ptr);
    assert(buf.len == orig_buf_len);
    assert(ring_buffer.allocation == orig_ring_alloc);
    assert(ring_buffer.allocation_end == orig_ring_alloc_end);

    /* Post‑condition: return value matches the membership predicate */
    assert(rval == belongs);
}
