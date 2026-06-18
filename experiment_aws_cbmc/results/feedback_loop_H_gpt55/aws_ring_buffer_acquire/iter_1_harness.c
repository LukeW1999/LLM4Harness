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

    size_t allocation_size = nondet_size_t();
    __CPROVER_assume(allocation_size > 0);
    __CPROVER_assume(allocation_size <= MAX_BUFFER_SIZE);

    ring_buf.allocator = aws_default_allocator();
    ring_buf.allocation = malloc(allocation_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + allocation_size;

    size_t head_offset = nondet_size_t();
    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(head_offset <= allocation_size);
    __CPROVER_assume(tail_offset <= allocation_size);

    aws_atomic_init_ptr(&ring_buf.head, ring_buf.allocation + head_offset);
    aws_atomic_init_ptr(&ring_buf.tail, ring_buf.allocation + tail_offset);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_byte_buf dest;
    dest.allocator = aws_default_allocator();
    dest.capacity = nondet_size_t();
    __CPROVER_assume(dest.capacity <= MAX_BUFFER_SIZE);
    dest.len = nondet_size_t();
    __CPROVER_assume(dest.len <= dest.capacity);
    if (dest.capacity == 0) {
        dest.buffer = NULL;
    } else {
        dest.buffer = malloc(dest.capacity);
        __CPROVER_assume(dest.buffer != NULL);
    }
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);

    struct aws_allocator *old_allocator = ring_buf.allocator;
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    uint8_t *old_head = aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *old_tail = aws_atomic_load_ptr(&ring_buf.tail);

    struct aws_byte_buf old_dest = dest;

    struct store_byte_from_buffer old_ring_byte;
    save_byte_from_array(ring_buf.allocation, allocation_size, &old_ring_byte);

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    uint8_t *new_head = aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *new_tail = aws_atomic_load_ptr(&ring_buf.tail);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (old_head == old_tail) {
        size_t ring_space = (size_t)(old_allocation_end - old_allocation);
        if (requested_size <= ring_space) {
            assert(result == AWS_OP_SUCCESS);
        } else {
            assert(result == AWS_OP_ERR);
        }
    } else if (old_tail > old_head) {
        size_t space = (size_t)(old_tail - old_head - 1);
        if (space >= requested_size) {
            assert(result == AWS_OP_SUCCESS);
        } else {
            assert(result == AWS_OP_ERR);
        }
    } else if (old_tail < old_head) {
        size_t head_space = (size_t)(old_allocation_end - old_head);
        size_t tail_space = (size_t)(old_tail - old_allocation);
        if (head_space >= requested_size || tail_space > requested_size) {
            assert(result == AWS_OP_SUCCESS);
        } else {
            assert(result == AWS_OP_ERR);
        }
    }

    if (result == AWS_OP_SUCCESS) {
        assert(dest.allocator == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == requested_size);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));

        if (old_head == old_tail) {
            assert(new_head == old_allocation + requested_size);
            assert(new_tail == old_allocation);
            assert(dest.buffer == old_allocation);
        } else if (old_tail > old_head) {
            assert((size_t)(old_tail - old_head - 1) >= requested_size);
            assert(new_head == old_head + requested_size);
            assert(new_tail == old_tail);
            assert(dest.buffer == old_head);
        } else if (old_tail < old_head) {
            if ((size_t)(old_allocation_end - old_head) >= requested_size) {
                assert(new_head == old_head + requested_size);
                assert(new_tail == old_tail);
                assert(dest.buffer == old_head);
            } else {
                assert((size_t)(old_tail - old_allocation) > requested_size);
                assert(new_head == old_allocation + requested_size);
                assert(new_tail == old_tail);
                assert(dest.buffer == old_allocation);
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
    }

    assert(ring_buf.allocator == old_allocator);
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);

    assert_byte_from_buffer_matches(ring_buf.allocation, &old_ring_byte);

    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
}
