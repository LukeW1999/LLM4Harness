#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_release_harness() {
    /* Non-deterministic size for ring buffer allocation */
    size_t size;
    __CPROVER_assume(size > 0 && size <= 1000000);

    /* Initialize ring buffer */
    struct aws_ring_buffer ring_buffer;
    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Nondeterministic size to acquire */
    size_t requested_size;
    __CPROVER_assume(requested_size > 0 && requested_size <= size);

    /* Initialize the destination buffer to a valid empty state */
    struct aws_byte_buf buf;
    buf.allocator = NULL;
    buf.buffer = NULL;
    buf.len = 0;
    buf.capacity = 0;

    /* Acquire a buffer from the ring buffer */
    int acquire_result = aws_ring_buffer_acquire(&ring_buffer, requested_size, &buf);
    __CPROVER_assume(acquire_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* Call the function */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Postconditions */
    assert(buf.allocator == NULL);
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);

    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
