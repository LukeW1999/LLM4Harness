#include <proof_helpers/make_common_data_structures.h>

#define MAX_RING_BUFFER_CLEAN_UP_SIZE 16

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size;

    __CPROVER_assume(aws_allocator_is_valid(allocator));
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size <= MAX_RING_BUFFER_CLEAN_UP_SIZE);

    int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    aws_ring_buffer_clean_up(&ring_buf);

    __CPROVER_assert(aws_ring_buffer_is_valid(&ring_buf), "ring buffer is valid after clean up");
    __CPROVER_assert(ring_buf.allocator == NULL, "allocator is NULL after clean up");
    __CPROVER_assert(ring_buf.allocation.buffer == NULL, "allocation buffer is NULL after clean up");
    __CPROVER_assert(ring_buf.allocation.len == 0, "allocation length is zero after clean up");
    __CPROVER_assert(ring_buf.allocation.capacity == 0, "allocation capacity is zero after clean up");
    __CPROVER_assert(ring_buf.allocation.allocator == NULL, "allocation allocator is NULL after clean up");
    __CPROVER_assert(ring_buf.head == NULL, "head is NULL after clean up");
    __CPROVER_assert(ring_buf.tail == NULL, "tail is NULL after clean up");
}
