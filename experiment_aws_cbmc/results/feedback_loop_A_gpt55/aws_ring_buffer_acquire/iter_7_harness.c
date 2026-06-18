#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

#define ASSERT(expr) __CPROVER_assert((expr), #expr)

void aws_ring_buffer_acquire_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_ring_buffer ring_buf = {0};

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 1);
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    uint8_t *allocation = bounded_malloc(capacity);
    __CPROVER_assume(allocation != NULL);

    ring_buf.allocator = allocator;
    ring_buf.allocation = allocation;
    ring_buf.allocation_end = allocation + capacity;

    size_t head_offset = nondet_size_t();
    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(head_offset < capacity);
    __CPROVER_assume(tail_offset < capacity);

    uint8_t *head_ptr = allocation + head_offset;
    uint8_t *tail_ptr = allocation + tail_offset;

    aws_atomic_init_ptr(&ring_buf.head, head_ptr);
    aws_atomic_init_ptr(&ring_buf.tail, tail_ptr);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_byte_buf dest = {0};

    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size < capacity);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    struct aws_allocator *old_allocator = ring_buf.allocator;
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *old_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);

    ASSERT(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
    ASSERT(ring_buf.allocator == old_allocator);
    ASSERT(ring_buf.allocation == old_allocation);
    ASSERT(ring_buf.allocation_end == old_allocation_end);
    ASSERT(new_tail == old_tail);
    ASSERT(aws_ring_buffer_is_valid(&ring_buf));

    if (result == AWS_OP_SUCCESS) {
        ASSERT(dest.len == 0);
        ASSERT(dest.capacity == requested_size);
        ASSERT(dest.allocator == NULL);
        ASSERT(dest.buffer != NULL);
        ASSERT(aws_byte_buf_is_valid(&dest));
        ASSERT(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));

        if (old_tail > old_head) {
            size_t available_space = (size_t)(old_tail - old_head - 1);
            ASSERT(available_space >= requested_size);
            ASSERT(dest.buffer == old_head);
            ASSERT(new_head == old_head + requested_size);
        } else {
            size_t head_space = (size_t)(old_allocation_end - old_head);
            if (head_space >= requested_size) {
                ASSERT(dest.buffer == old_head);
                if (head_space == requested_size) {
                    ASSERT(new_head == old_allocation);
                } else {
                    ASSERT(new_head == old_head + requested_size);
                }
            } else {
                size_t tail_space = (size_t)(old_tail - old_allocation);
                ASSERT(tail_space > requested_size);
                ASSERT(dest.buffer == old_allocation);
                ASSERT(new_head == old_allocation + requested_size);
            }
        }
    }
}
