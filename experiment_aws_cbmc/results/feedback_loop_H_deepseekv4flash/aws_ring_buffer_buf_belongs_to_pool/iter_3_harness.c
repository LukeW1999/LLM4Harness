#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* Non-deterministic structures */
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* Set up ring buffer: allocation must be valid */
    size_t allocation_size;
    __CPROVER_assume(allocation_size > 0 && allocation_size <= MAX_BUFFER_SIZE);
    ring_buffer.allocation = (uint8_t *)malloc(allocation_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + allocation_size;

    /* allocator is not used by belongs_to_pool, but we initialize for completeness */
    ring_buffer.allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(ring_buffer.allocator != NULL);

    /* Initialize head and tail (atomic) - these are not used by the function */
    aws_atomic_init_int(&ring_buffer.head, nondet_size_t());
    aws_atomic_init_int(&ring_buffer.tail, nondet_size_t());

    /* Set up byte buf: ensure buffer is valid */
    buf.allocator = ring_buffer.allocator;
    buf.buffer = (uint8_t *)malloc(sizeof(uint8_t) * MAX_BUFFER_SIZE);
    __CPROVER_assume(buf.buffer != NULL);
    buf.capacity = MAX_BUFFER_SIZE;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    /* Ensure byte_buf is valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* Call the function */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* Postconditions: ring_buffer fields unchanged */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    assert(aws_atomic_load_int(&ring_buffer.head) == aws_atomic_load_int(&old_ring_buffer.head));
    assert(aws_atomic_load_int(&ring_buffer.tail) == aws_atomic_load_int(&old_ring_buffer.tail));

    /* Postconditions: byte_buf fields unchanged */
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);

    /* Postconditions: ring_buffer remains valid */
    assert(ring_buffer.allocation != NULL);
    assert(ring_buffer.allocation_end > ring_buffer.allocation);
    assert(ring_buffer.allocator != NULL);

    /* Postconditions: byte_buf remains valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* If result is true, buf->buffer must be wholly within the ring buffer's allocation range */
    if (result) {
        assert(buf.buffer >= ring_buffer.allocation && buf.buffer < ring_buffer.allocation_end);
        assert(buf.buffer + buf.len <= ring_buffer.allocation_end);
    } else {
        /* If result is false, buf->buffer is not within the ring buffer's allocation range */
        assert(buf.buffer < ring_buffer.allocation || buf.buffer >= ring_buffer.allocation_end);
    }
}
