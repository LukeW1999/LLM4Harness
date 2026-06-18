#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* 1. Declare stack‑allocated structures */
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf   buf;

    /* 2. Allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 3. Ring buffer setup (only allocation matters for this function) */
    ring_buffer.allocator      = allocator;
    ring_buffer.allocation     = NULL;
    ring_buffer.allocation_end = NULL;

    /* Backing memory for the ring buffer */
    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);
    ring_buffer.allocation = (uint8_t *)malloc(ring_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + ring_size;

    /* head/tail are irrelevant; we can leave them uninitialised */
    /* (the function under test does not read them)                */

    /* 4. Byte buffer setup */
    buf.allocator = allocator;
    buf.buffer    = NULL;
    buf.len       = 0;
    buf.capacity  = 0;

    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 5. Save old state for immutability checks */
    struct aws_ring_buffer old_ring  = ring_buffer;
    struct aws_byte_buf   old_buf   = buf;

    /* 6. Call the function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* 7. Postconditions on the return value */
    if (result) {
        assert(buf.buffer >= ring_buffer.allocation);
        assert(buf.buffer <  ring_buffer.allocation_end);
    } else {
        assert(buf.buffer <  ring_buffer.allocation ||
               buf.buffer >= ring_buffer.allocation_end);
    }

    /* 8. Immutability: the function must not change any input fields */
    assert(ring_buffer.allocator      == old_ring.allocator);
    assert(ring_buffer.allocation     == old_ring.allocation);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);

    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer    == old_buf.buffer);
    assert(buf.len       == old_buf.len);
    assert(buf.capacity  == old_buf.capacity);

    /* 9. Input validity is unchanged */
    assert(aws_byte_buf_is_valid(&buf));
    /* The ring buffer’s allocation is still valid, but we do not call
       aws_ring_buffer_is_valid because head/tail may be uninitialised.
       That is acceptable because the function does not depend on them. */

    /* 10. Cleanup */
    free(ring_buffer.allocation);
    free(buf.buffer);
}
