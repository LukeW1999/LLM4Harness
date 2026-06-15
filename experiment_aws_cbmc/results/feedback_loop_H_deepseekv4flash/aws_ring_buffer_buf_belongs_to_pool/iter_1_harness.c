#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* Non-deterministic structures */
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* Ensure ring buffer validity */
    ring_buffer.allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    /* allocation and allocation_end should be valid pointers */
    size_t allocation_size = nondet_size_t();
    __CPROVER_assume(allocation_size > 0 && allocation_size <= MAX_BUFFER_SIZE);
    ring_buffer.allocation = (uint8_t *)malloc(allocation_size);
    ring_buffer.allocation_end = ring_buffer.allocation + allocation_size;
    __CPROVER_assume(ring_buffer.allocation != NULL);
    __CPROVER_assume(ring_buffer.allocator != NULL);
    /* head and tail are atomic, but we only need them to be initialized */
    aws_atomic_init_int(&ring_buffer.head, nondet_size_t());
    aws_atomic_init_int(&ring_buffer.tail, nondet_size_t());

    /* Ensure byte_buf validity */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* Call the function */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* Assert unchanged fields of ring_buffer */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    /* head and tail are unchanged as well */
    assert(aws_atomic_load_int(&ring_buffer.head) == aws_atomic_load_int(&old_ring_buffer.head));
    assert(aws_atomic_load_int(&ring_buffer.tail) == aws_atomic_load_int(&old_ring_buffer.tail));

    /* Assert unchanged fields of byte_buf */
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);

    /* Assert ring buffer is still valid */
    assert(ring_buffer.allocation != NULL);
    assert(ring_buffer.allocation_end > ring_buffer.allocation);
    assert(ring_buffer.allocator != NULL);

    /* Assert byte_buf is still valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* If result is true, buf->buffer must lie within the ring buffer's allocation range */
    if (result) {
        /* Must be at least one byte within range */
        assert(buf.buffer >= ring_buffer.allocation && buf.buffer < ring_buffer.allocation_end);
        /* The entire buffer must fit (len may be 0, still considered within) */
        if (buf.len > 0) {
            assert(buf.buffer + buf.len <= ring_buffer.allocation_end);
        }
    }
}
