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
 * 2. Unchanged fields: ring_buffer and buf are both const — nothing changes
 * 3. Failure path: returns false (not an error code, just a bool)
 * 4. Validity invariants: ring_buffer remains valid after the call
 */

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* 1. Set up the ring buffer */
    struct aws_ring_buffer ring_buf;

    /* We need a valid ring buffer. Use aws_ring_buffer_init to create one. */
    size_t size;
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Set up the byte buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 3. Snapshot state before the call */
    /* Ring buffer fields */
    struct aws_allocator *old_rb_allocator = ring_buf.allocator;
    uint8_t *old_rb_allocation = ring_buf.allocation;
    uint8_t *old_rb_allocation_end = ring_buf.allocation_end;

    /* Byte buf fields */
    uint8_t *old_buf_buffer = buf.buffer;
    size_t old_buf_len = buf.len;
    size_t old_buf_capacity = buf.capacity;
    struct aws_allocator *old_buf_allocator = buf.allocator;

    /* 4. Call the function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    /* 5. Assert postconditions */

    /* RETURN VALUE: result is a bool — either true or false */
    /* If true, buf->buffer must be within [ring_buf.allocation, ring_buf.allocation_end) */
    if (result) {
        assert(buf.buffer != NULL);
        assert(buf.buffer >= ring_buf.allocation);
        assert(buf.buffer < ring_buf.allocation_end);
    }
    /* If false, either buf->buffer is NULL or outside the ring buffer range */
    /* (no additional assert needed beyond the bool value itself) */

    /* FRAME: ring_buffer must not be modified (it's const) */
    assert(ring_buf.allocator == old_rb_allocator);
    assert(ring_buf.allocation == old_rb_allocation);
    assert(ring_buf.allocation_end == old_rb_allocation_end);

    /* FRAME: buf must not be modified (it's const) */
    assert(buf.buffer == old_buf_buffer);
    assert(buf.len == old_buf_len);
    assert(buf.capacity == old_buf_capacity);
    assert(buf.allocator == old_buf_allocator);

    /* INVARIANTS: ring buffer remains valid */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    /* INVARIANTS: byte buf remains valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buf);
}
