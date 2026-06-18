#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>

void aws_ring_buffer_release_harness(void) {
    /* Declare the data structures */
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf buf;

    /* Use the default allocator for any allocator fields */
    ring_buf.allocator = aws_default_allocator();

    /* Save original values for later comparison */
    struct aws_atomic_var orig_head = ring_buf.head;
    struct aws_atomic_var orig_tail = ring_buf.tail;
    uint8_t *orig_allocation = ring_buf.allocation;
    uint8_t *orig_allocation_end = ring_buf.allocation_end;
    struct aws_allocator *orig_allocator = ring_buf.allocator;

    struct aws_byte_buf orig_buf = buf;

    /* Preconditions supplied by the expert */
    __CPROVER_assume(!aws_ring_buffer_is_empty(&ring_buf));
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Call the function under verification */
    aws_ring_buffer_release(&ring_buf, &buf);

    /* -------------------------------------------------------------------- */
    /* Post‑condition checks */

    /* 1. The ring buffer must still satisfy its validity predicate */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. The tail pointer must be updated to point just past the released buffer */
    assert(aws_atomic_load_ptr(&ring_buf.tail) ==
           (void *)(orig_buf.buffer + orig_buf.capacity));

    /* 3. No other fields of the ring buffer may have been modified */
    assert(aws_atomic_load_ptr(&ring_buf.head) ==
           aws_atomic_load_ptr(&orig_head));
    assert(ring_buf.allocation == orig_allocation);
    assert(ring_buf.allocation_end == orig_allocation_end);
    assert(ring_buf.allocator == orig_allocator);

    /* 4. The released byte buffer must be zeroed out */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    return 0;
}
