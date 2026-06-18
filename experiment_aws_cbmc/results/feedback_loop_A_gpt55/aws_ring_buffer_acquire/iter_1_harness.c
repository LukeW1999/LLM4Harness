#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(capacity <= (size_t)PTRDIFF_MAX);

    ring_buf.allocator = aws_default_allocator();
    ring_buf.allocation = malloc(capacity);
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
    __CPROVER_assume(dest_capacity <= (size_t)PTRDIFF_MAX);

    size_t dest_len = nondet_size_t();
    __CPROVER_assume(dest_len <= dest_capacity);

    dest.allocator = aws_default_allocator();
    dest.buffer = malloc(dest_capacity);
    __CPROVER_assume(dest.buffer != NULL);
    dest.len = dest_len;
    dest.capacity = dest_capacity;

    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(requested_size <= (size_t)PTRDIFF_MAX);

    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_dest = dest;

    uint8_t *old_head = aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *old_tail = aws_atomic_load_ptr(&ring_buf.tail);
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    size_t old_ring_space = (size_t)(old_allocation_end - old_allocation);

    struct store_byte_from_buffer ring_storage;
    save_byte_from_array(ring_buf.allocation, capacity, &ring_storage);

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    uint8_t *new_head = aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *new_tail = aws_atomic_load_ptr(&ring_buf.tail);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == 0);
        assert(dest.capacity == requested_size);
        assert(dest.allocator == NULL);
        assert(dest.buffer != NULL);
        assert(AWS_MEM_IS_WRITABLE(dest.buffer, dest.capacity));
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));

        if (old_head == old_tail) {
            assert(requested_size <= old_ring_space);
            assert(dest.buffer == old_allocation);
            assert(new_head == old_allocation + requested_size);
            assert(new_tail == old_allocation);
        } else if (old_tail > old_head) {
            size_t available_space = (size_t)(old_tail - old_head - 1);
            assert(available_space >= requested_size);
            assert(dest.buffer == old_head);
            assert(new_head == old_head + requested_size);
            assert(new_tail == old_tail);
        } else if (old_tail < old_head) {
            size_t head_space = (size_t)(old_allocation_end - old_head);
            if (head_space >= requested_size) {
                assert(dest.buffer == old_head);
                assert(new_head == old_head + requested_size);
                assert(new_tail == old_tail);
            } else {
                size_t tail_space = (size_t)(old_tail - old_allocation);
                assert(tail_space > requested_size);
                assert(dest.buffer == old_allocation);
                assert(new_head == old_allocation + requested_size);
                assert(new_tail == old_tail);
            }
        }
    } else {
        assert(result == AWS_OP_ERR);

        assert(new_head == old_head);
        assert(new_tail == old_tail);

        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);

        if (old_head == old_tail) {
            assert(requested_size > old_ring_space);
        } else if (old_tail > old_head) {
            size_t available_space = (size_t)(old_tail - old_head - 1);
            assert(available_space < requested_size);
        } else if (old_tail < old_head) {
            size_t head_space = (size_t)(old_allocation_end - old_head);
            size_t tail_space = (size_t)(old_tail - old_allocation);
            assert(head_space < requested_size);
            assert(tail_space <= requested_size);
        }
    }

    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);

    assert_byte_from_buffer_matches(ring_buf.allocation, &ring_storage);

    assert(aws_ring_buffer_check_atomic_ptr(&ring_buf, new_head));
    assert(aws_ring_buffer_check_atomic_ptr(&ring_buf, new_tail));
    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_valid(&dest));
}
