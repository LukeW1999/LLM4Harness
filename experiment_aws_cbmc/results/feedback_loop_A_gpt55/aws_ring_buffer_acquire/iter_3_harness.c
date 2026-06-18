#include <proof_helpers/make_common_data_structures.h>

#define ASSERT(expr) __CPROVER_assert((expr), #expr)

void aws_ring_buffer_acquire_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_ring_buffer ring_buf;

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    ring_buf.allocator = allocator;
    ring_buf.allocation = bounded_malloc(capacity);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + capacity;

    size_t head_offset = nondet_size_t();
    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(head_offset <= capacity);
    __CPROVER_assume(tail_offset <= capacity);

    uint8_t *head_ptr = ring_buf.allocation + head_offset;
    uint8_t *tail_ptr = ring_buf.allocation + tail_offset;

    aws_atomic_init_ptr(&ring_buf.head, head_ptr);
    aws_atomic_init_ptr(&ring_buf.tail, tail_ptr);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_byte_buf dest;

    size_t dest_capacity = nondet_size_t();
    __CPROVER_assume(dest_capacity > 0);
    __CPROVER_assume(dest_capacity <= MAX_BUFFER_SIZE);

    size_t dest_len = nondet_size_t();
    __CPROVER_assume(dest_len <= dest_capacity);

    dest.allocator = allocator;
    dest.buffer = bounded_malloc(dest_capacity);
    __CPROVER_assume(dest.buffer != NULL);
    dest.len = dest_len;
    dest.capacity = dest_capacity;

    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    struct aws_byte_buf old_dest = dest;

    struct aws_allocator *old_allocator = ring_buf.allocator;
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *old_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);

    struct store_byte_from_buffer ring_storage;
    save_byte_from_array(ring_buf.allocation, capacity, &ring_storage);

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);

    ASSERT(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        ASSERT(dest.len == 0);
        ASSERT(dest.capacity == requested_size);
        ASSERT(dest.allocator == NULL);
        ASSERT(dest.buffer != NULL);
        ASSERT(AWS_MEM_IS_WRITABLE(dest.buffer, dest.capacity));
        ASSERT(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));

        if (old_tail > old_head) {
            size_t available_space = (size_t)(old_tail - old_head - 1);
            ASSERT(available_space >= requested_size);
            ASSERT(dest.buffer == old_head);
            ASSERT(new_head == old_head + requested_size);
            ASSERT(new_tail == old_tail);
        } else {
            size_t head_space = (size_t)(old_allocation_end - old_head);
            if (head_space >= requested_size) {
                ASSERT(dest.buffer == old_head);
                ASSERT(new_head == old_head + requested_size);
                ASSERT(new_tail == old_tail);
            } else {
                size_t tail_space = (size_t)(old_tail - old_allocation);
                ASSERT(tail_space > requested_size);
                ASSERT(dest.buffer == old_allocation);
                ASSERT(new_head == old_allocation + requested_size);
                ASSERT(new_tail == old_tail);
            }
        }
    } else {
        ASSERT(new_head == old_head);
        ASSERT(new_tail == old_tail);

        ASSERT(dest.allocator == old_dest.allocator);
        ASSERT(dest.buffer == old_dest.buffer);
        ASSERT(dest.len == old_dest.len);
        ASSERT(dest.capacity == old_dest.capacity);

        if (old_tail > old_head) {
            size_t available_space = (size_t)(old_tail - old_head - 1);
            ASSERT(available_space < requested_size);
        } else {
            size_t head_space = (size_t)(old_allocation_end - old_head);
            size_t tail_space = (size_t)(old_tail - old_allocation);
            ASSERT(head_space < requested_size);
            ASSERT(tail_space <= requested_size);
        }
    }

    ASSERT(ring_buf.allocator == old_allocator);
    ASSERT(ring_buf.allocation == old_allocation);
    ASSERT(ring_buf.allocation_end == old_allocation_end);

    assert_byte_from_buffer_matches(ring_buf.allocation, &ring_storage);

    ASSERT(aws_ring_buffer_check_atomic_ptr(&ring_buf, &ring_buf.head));
    ASSERT(aws_ring_buffer_check_atomic_ptr(&ring_buf, &ring_buf.tail));
    ASSERT(aws_ring_buffer_is_valid(&ring_buf));
    ASSERT(aws_byte_buf_is_valid(&dest));
}
