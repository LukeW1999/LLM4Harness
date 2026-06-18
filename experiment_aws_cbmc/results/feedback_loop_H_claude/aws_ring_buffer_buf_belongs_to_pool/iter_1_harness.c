#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * Harness for aws_ring_buffer_buf_belongs_to_pool
 *
 * Doxygen says:
 *   Returns true if the memory in `buf` was vended by this ring buffer, false otherwise.
 *   Make sure `buf->buffer` and `ring_buffer->allocation` refer to the same memory region.
 *
 * Analysis:
 * 1. Changed fields on success: none — this is a pure predicate (returns bool)
 * 2. Unchanged fields: all fields of ring_buffer and buf remain unchanged
 * 3. Failure: returns false (no error code, just bool)
 * 4. Validity invariants: ring_buffer and buf remain valid after the call
 */

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* Set up ring buffer */
    struct aws_ring_buffer ring_buf;

    /* We need a valid ring buffer with a real allocation */
    size_t size;
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Set up byte buf non-deterministically */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_buf = buf;

    /* Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    /* Postconditions:
     * 1. The function returns true iff buf->buffer is within ring_buf's allocation range.
     *    We can assert the logical equivalence:
     *    result == true implies buf.buffer >= ring_buf.allocation && buf.buffer < ring_buf.allocation_end
     *    result == false implies buf.buffer is outside that range (or buf.buffer is NULL)
     */
    if (result) {
        /* buf belongs to pool: buffer must be within allocation bounds */
        assert(buf.buffer != NULL);
        assert(buf.buffer >= ring_buf.allocation);
        assert(buf.buffer < ring_buf.allocation_end);
    }

    /* 2. Unchanged fields of ring_buf */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);

    /* 3. Unchanged fields of buf */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* 4. Validity invariants */
    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_valid(&buf));

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buf);
}
