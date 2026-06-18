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
 * 3. Failure: returns false (not an error code, just a boolean)
 * 4. Validity invariants: ring_buffer and buf remain valid after the call
 */

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* Set up ring buffer */
    struct aws_ring_buffer ring_buffer;

    /* We need a valid ring buffer with a real allocation */
    size_t size;
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Set up byte buf */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* Postconditions:
     * The function is a pure predicate — it should not modify ring_buffer or buf.
     * Result is true iff buf->buffer is within [ring_buffer->allocation, ring_buffer->allocation_end)
     */

    /* If result is true, buf->buffer must be within the ring buffer's allocation range */
    if (result) {
        assert(buf.buffer != NULL);
        assert(buf.buffer >= ring_buffer.allocation);
        assert(buf.buffer < ring_buffer.allocation_end);
    }

    /* Ring buffer fields must be unchanged */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);

    /* buf fields must be unchanged */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* Validity invariants must still hold */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buffer);
}
