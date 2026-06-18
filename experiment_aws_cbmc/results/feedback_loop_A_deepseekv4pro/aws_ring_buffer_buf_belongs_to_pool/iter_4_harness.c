#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* Allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize ring_buffer with non-deterministic values */
    ring_buffer.allocator = allocator;
    ring_buffer.allocation = NULL;
    ring_buffer.allocation_end = NULL;
    aws_atomic_init_int(&ring_buffer.head, 0);
    aws_atomic_init_int(&ring_buffer.tail, 0);

    /* Allocate the ring buffer's backing memory non-deterministically */
    size_t ring_size;
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);
    ring_buffer.allocation = (uint8_t *)malloc(ring_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + ring_size;

    /* Set head and tail within bounds */
    size_t head_val;
    size_t tail_val;
    __CPROVER_assume(head_val < ring_size);
    __CPROVER_assume(tail_val < ring_size);
    aws_atomic_store_int(&ring_buffer.head, head_val);
    aws_atomic_store_int(&ring_buffer.tail, tail_val);

    /* Ensure ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Initialize buf with non-deterministic values */
    buf.allocator = allocator;
    buf.buffer = NULL;
    buf.len = 0;
    buf.capacity = 0;

    /* Allocate buf's buffer member non-deterministically */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* 4. Assert postconditions */

    /* The function returns true if buf->buffer is within ring_buffer's allocation range */
    if (result) {
        /* buf->buffer must be within [ring_buffer.allocation, ring_buffer.allocation_end) */
        assert(buf.buffer >= ring_buffer.allocation);
        assert(buf.buffer < ring_buffer.allocation_end);
    } else {
        /* buf->buffer is outside the ring buffer's allocation range */
        assert(buf.buffer < ring_buffer.allocation || buf.buffer >= ring_buffer.allocation_end);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* ring_buffer fields are all read-only in this function */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    assert(aws_atomic_load_int(&ring_buffer.head) == aws_atomic_load_int(&old_ring_buffer.head));
    assert(aws_atomic_load_int(&ring_buffer.tail) == aws_atomic_load_int(&old_ring_buffer.tail));

    /* buf fields are all read-only in this function */
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);

    /* 6. Assert validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));

    /* Clean up */
    free(ring_buffer.allocation);
    free(buf.buffer);
}
