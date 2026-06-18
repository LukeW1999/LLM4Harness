#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness() {
    /* 1. Set up ring buffer */
    struct aws_ring_buffer ring_buffer;
    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);

    /* Initialize the ring buffer */
    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, ring_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* 2. Acquire a buf from the ring buffer */
    struct aws_byte_buf buf;
    size_t requested_size;
    __CPROVER_assume(requested_size > 0 && requested_size <= ring_size);

    int acquire_result = aws_ring_buffer_acquire(&ring_buffer, requested_size, &buf);
    __CPROVER_assume(acquire_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    /* Save old state */
    uint8_t *expected_new_tail = buf.buffer + buf.capacity;

    /* 3. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 4. Assert postconditions */

    /* The buf should be zeroed out after release */
    __CPROVER_assert(buf.buffer == NULL, "buf.buffer is NULL after release");
    __CPROVER_assert(buf.len == 0, "buf.len is 0 after release");
    __CPROVER_assert(buf.capacity == 0, "buf.capacity is 0 after release");
    __CPROVER_assert(buf.allocator == NULL, "buf.allocator is NULL after release");

    /* Ring buffer validity */
    __CPROVER_assert(aws_ring_buffer_is_valid(&ring_buffer), "ring buffer is valid after release");

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buffer);
}
