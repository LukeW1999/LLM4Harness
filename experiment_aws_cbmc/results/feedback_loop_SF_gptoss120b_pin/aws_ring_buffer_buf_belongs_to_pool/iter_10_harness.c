#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize ring buffer allocation */
    size_t ring_size;
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= 256);
    uint8_t *ring_mem = (uint8_t *)malloc(ring_size);
    __CPROVER_assume(ring_mem != NULL);
    ring_buffer.allocation = ring_mem;
    ring_buffer.allocation_end = ring_mem + ring_size;
    ring_buffer.head = 0;
    ring_buffer.tail = 0;
    ring_buffer.allocation_size = ring_size;
    ring_buffer.is_dynamic = false;
    ring_buffer.is_shrinking = false;
    ring_buffer.is_growing = false;
    ring_buffer.is_full = false;
    ring_buffer.is_empty = true;

    /* Symbolic byte buffer */
    ensure_byte_buf_is_valid(&buf, allocator);

    /* Preconditions */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Additional assumptions matching the function's expectations */
    __CPROVER_assume(
        buf.buffer == NULL ||
        (buf.buffer >= ring_buffer.allocation &&
         buf.buffer < ring_buffer.allocation_end &&
         (size_t)(buf.buffer - ring_buffer.allocation) + buf.capacity <=
             (size_t)(ring_buffer.allocation_end - ring_buffer.allocation)));
    __CPROVER_assume(buf.buffer == NULL || buf.capacity > 0);

    /* Snapshot of inputs */
    const uint8_t *orig_buf_ptr = buf.buffer;
    size_t orig_buf_len = buf.len;
    const uint8_t *orig_ring_alloc = ring_buffer.allocation;
    const uint8_t *orig_ring_alloc_end = ring_buffer.allocation_end;

    /* Compute expected result */
    bool belongs = false;
    if (orig_buf_ptr != NULL) {
        if (orig_buf_ptr >= ring_buffer.allocation && orig_buf_ptr < ring_buffer.allocation_end) {
            size_t offset = (size_t)(orig_buf_ptr - ring_buffer.allocation);
            if (offset + buf.capacity <= (size_t)(ring_buffer.allocation_end - ring_buffer.allocation)) {
                belongs = true;
            }
        }
    }

    /* Call under verification */
    bool rval = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* Frame conditions */
    assert(buf.buffer == orig_buf_ptr);
    assert(buf.len == orig_buf_len);
    assert(ring_buffer.allocation == orig_ring_alloc);
    assert(ring_buffer.allocation_end == orig_ring_alloc_end);

    /* Post‑condition */
    assert(rval == belongs);
}
