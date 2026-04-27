/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* Set up ring buffer */
    struct aws_ring_buffer *ring_buffer = malloc(sizeof(*ring_buffer));
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size;

    /* Preconditions for ring buffer */
    __CPROVER_assume(ring_buffer != NULL);
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(size > 0 && size < MAX_MALLOC);

    /* Initialize the ring buffer so it is valid */
    if (aws_ring_buffer_init(ring_buffer, allocator, size) != AWS_OP_SUCCESS) {
        return;
    }
    __CPROVER_assume(aws_ring_buffer_is_valid(ring_buffer));

    /* Set up byte buf */
    struct aws_byte_buf *buf = malloc(sizeof(*buf));
    __CPROVER_assume(buf != NULL);
    ensure_byte_buf_has_allocated_buffer_member(buf);
    __CPROVER_assume(aws_byte_buf_is_valid(buf));

    /* Save old state */
    struct aws_ring_buffer old_ring = *ring_buffer;
    struct aws_byte_buf old_buf = *buf;

    /* Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(ring_buffer, buf);

    /* Postconditions:
     * 1. The function returns true if buf->buffer is within [ring_buffer->allocation, ring_buffer->allocation_end)
     * 2. The ring buffer must not be modified
     * 3. The buf must not be modified
     * 4. Validity invariants must hold
     */

    /* Check return value semantics */
    if (result) {
        /* buf->buffer must be within the ring buffer's allocation range */
        assert(buf->buffer >= ring_buffer->allocation);
        assert(buf->buffer < ring_buffer->allocation_end);
    }
    /* We do not assert the else branch because buf->buffer could be NULL or
     * point anywhere when result is false - the function only guarantees
     * the positive case */

    /* Ring buffer fields must not change */
    assert(ring_buffer->allocator == old_ring.allocator);
    assert(ring_buffer->allocation == old_ring.allocation);
    assert(ring_buffer->allocation_end == old_ring.allocation_end);

    /* buf fields must not change */
    assert(buf->buffer == old_buf.buffer);
    assert(buf->len == old_buf.len);
    assert(buf->capacity == old_buf.capacity);
    assert(buf->allocator == old_buf.allocator);

    /* Validity invariants */
    assert(aws_ring_buffer_is_valid(ring_buffer));
    assert(aws_byte_buf_is_valid(buf));
}
