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
    struct aws_byte_buf dest;

    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);

    uint8_t *allocation = malloc(ring_size);
    __CPROVER_assume(allocation != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(allocation, ring_size));

    size_t head_offset = nondet_size_t();
    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(head_offset <= ring_size);
    __CPROVER_assume(tail_offset <= ring_size);

    ring_buf.allocator = aws_default_allocator();
    ring_buf.allocation = allocation;
    ring_buf.allocation_end = allocation + ring_size;
    aws_atomic_init_ptr(&ring_buf.head, allocation + head_offset);
    aws_atomic_init_ptr(&ring_buf.tail, allocation + tail_offset);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);

    uint8_t *old_head = aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *old_tail = aws_atomic_load_ptr(&ring_buf.tail);

    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_dest = dest;

    struct store_byte_from_buffer old_allocation_byte;
    save_byte_from_array(ring_buf.allocation, ring_size, &old_allocation_byte);

    bool can_acquire = false;
    uint8_t *expected_dest_buffer = NULL;
    uint8_t *expected_head = old_head;
    uint8_t *expected_tail = old_tail;

    if (old_head == old_tail) {
        if (requested_size <= ring_size) {
            can_acquire = true;
            expected_dest_buffer = ring_buf.allocation;
            expected_head = ring_buf.allocation + requested_size;
            expected_tail = ring_buf.allocation;
        }
    } else if (old_tail > old_head) {
        size_t space = (size_t)(old_tail - old_head - 1);
        if (space >= requested_size) {
            can_acquire = true;
            expected_dest_buffer = old_head;
            expected_head = old_head + requested_size;
            expected_tail = old_tail;
        }
    } else if (old_tail < old_head) {
        size_t head_space = (size_t)(ring_buf.allocation_end - old_head);
        size_t tail_space = (size_t)(old_tail - ring_buf.allocation);

        if (head_space >= requested_size) {
            can_acquire = true;
            expected_dest_buffer = old_head;
            expected_head = old_head + requested_size;
            expected_tail = old_tail;
        } else if (tail_space > requested_size) {
            can_acquire = true;
            expected_dest_buffer = ring_buf.allocation;
            expected_head = ring_buf.allocation + requested_size;
            expected_tail = old_tail;
        }
    }

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    uint8_t *new_head = aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *new_tail = aws_atomic_load_ptr(&ring_buf.tail);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(can_acquire);
        assert(dest.buffer == expected_dest_buffer);
        assert(dest.len == 0);
        assert(dest.capacity == requested_size);
        assert(dest.allocator == NULL);
        assert(new_head == expected_head);
        assert(new_tail == expected_tail);
        assert(AWS_MEM_IS_WRITABLE(dest.buffer, dest.capacity));
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
    } else {
        assert(result == AWS_OP_ERR);
        assert(!can_acquire);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(new_head == old_head);
        assert(new_tail == old_tail);
    }

    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);

    assert_byte_from_buffer_matches(ring_buf.allocation, &old_allocation_byte);

    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_valid(&dest));
}
