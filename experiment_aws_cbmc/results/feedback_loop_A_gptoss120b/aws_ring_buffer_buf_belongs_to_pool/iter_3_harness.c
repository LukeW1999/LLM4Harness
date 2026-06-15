#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <aws/common/ring_buffer.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 1024

int main(void) {
    /* 1. Allocate and initialize a ring buffer */
    struct aws_ring_buffer ring_buffer;
    size_t alloc_size;
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_BUFFER_SIZE);
    uint8_t *allocation = (uint8_t *)malloc(alloc_size);
    __CPROVER_assume(allocation != NULL);

    ring_buffer.allocator      = NULL;               /* allocator not used in this proof */
    ring_buffer.allocation     = allocation;
    ring_buffer.allocation_end = allocation + alloc_size;
    ring_buffer.head           = 0;
    ring_buffer.tail           = 0;
    ring_buffer.is_valid      = 1;                  /* assume valid flag if present */

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* 2. Prepare a byte buffer */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 3. Choose whether the buffer should belong to the pool */
    _Bool belongs;
    __CPROVER_assume(belongs == 0 || belongs == 1);

    if (belongs) {
        /* Place buf.buffer somewhere inside the ring buffer allocation */
        size_t offset;
        __CPROVER_assume(offset < alloc_size);
        buf.buffer = allocation + offset;
    } else {
        /* Place buf.buffer outside the ring buffer allocation */
        size_t offset;
        __CPROVER_assume(offset > 0);
        buf.buffer = allocation + alloc_size + offset;
    }

    /* 4. Save old state */
    struct aws_ring_buffer old_ring = ring_buffer;
    struct aws_byte_buf   old_buf   = buf;

    /* 5. Call the function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* 6. Assert that inputs are unchanged */
    assert(ring_buffer.allocator      == old_ring.allocator);
    assert(ring_buffer.allocation     == old_ring.allocation);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);
    assert(ring_buffer.head           == old_ring.head);
    assert(ring_buffer.tail           == old_ring.tail);
    assert(buf.allocator              == old_buf.allocator);
    assert(buf.buffer                 == old_buf.buffer);
    assert(buf.len                    == old_buf.len);
    assert(buf.capacity               == old_buf.capacity);

    /* 7. Assert the result matches the specification */
    bool expected = (buf.buffer >= ring_buffer.allocation) &&
                    (buf.buffer <  ring_buffer.allocation_end);
    assert(result == expected);

    /* 8. Re‑assert invariants */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    return 0;
}
