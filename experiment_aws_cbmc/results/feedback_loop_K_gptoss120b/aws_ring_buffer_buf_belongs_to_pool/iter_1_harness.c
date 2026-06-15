#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* Allocate and initialize a ring buffer */
    struct aws_ring_buffer *ring_buffer = malloc(sizeof(struct aws_ring_buffer));
    __CPROVER_assume(ring_buffer != NULL);

    struct aws_allocator *alloc = aws_default_allocator();
    size_t ring_size;
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= 1024 * 1024); /* bound for tractability */

    int init_res = aws_ring_buffer_init(ring_buffer, alloc, ring_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(ring_buffer));

    /* Make a copy of the ring buffer for frame checking */
    struct aws_ring_buffer ring_buffer_before = *ring_buffer;

    /* Allocate and set up a byte buffer */
    struct aws_byte_buf *buf = malloc(sizeof(struct aws_byte_buf));
    __CPROVER_assume(buf != NULL);

    /* Nondeterministically choose buffer pointer and capacity */
    uint8_t *buf_ptr;
    __CPROVER_assume(buf_ptr == NULL || __CPROVER_is_fresh(buf_ptr, 0));
    buf->buffer = buf_ptr;

    size_t cap;
    __CPROVER_assume(cap <= SIZE_MAX);
    buf->capacity = cap;
    buf->len = 0; /* len is irrelevant for the predicate */

    /* Make a copy of the byte buffer for frame checking */
    struct aws_byte_buf buf_before = *buf;

    /* Call the function under test */
    bool rval = aws_ring_buffer_buf_belongs_to_pool(ring_buffer, buf);

    /* Frame conditions: ring buffer and byte buffer must be unchanged */
    assert(ring_buffer->allocator   == ring_buffer_before.allocator);
    assert(ring_buffer->allocation  == ring_buffer_before.allocation);
    assert(ring_buffer->head        == ring_buffer_before.head);
    assert(ring_buffer->tail        == ring_buffer_before.tail);
    assert(ring_buffer->allocation_end == ring_buffer_before.allocation_end);

    assert(buf->buffer   == buf_before.buffer);
    assert(buf->capacity == buf_before.capacity);
    assert(buf->len      == buf_before.len);

    /* Compute the expected predicate */
    bool in_range = false;
    if (ring_buffer->allocation != NULL && ring_buffer->allocation_end != NULL && buf->buffer != NULL) {
        if (buf->buffer >= ring_buffer->allocation && buf->buffer <= ring_buffer->allocation_end) {
            size_t remaining = (size_t)(ring_buffer->allocation_end - buf->buffer);
            if (buf->capacity <= remaining) {
                in_range = true;
            }
        }
    }

    /* Postcondition: return value matches the predicate */
    assert(rval == in_range);

    /* Clean up */
    aws_ring_buffer_clean_up(ring_buffer);
    free(ring_buffer);
    free(buf);
    return 0;
}
