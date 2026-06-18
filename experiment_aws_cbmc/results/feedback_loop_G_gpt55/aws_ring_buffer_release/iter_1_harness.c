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

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring_buffer;
    size_t size = nondet_size_t();

    __CPROVER_assume(size > 0);
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    int init_result = aws_ring_buffer_init(&ring_buffer, aws_default_allocator(), size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    uint8_t *initial_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    __CPROVER_assume(initial_tail == ring_buffer.allocation);

    struct aws_byte_buf buf;
    size_t capacity = nondet_size_t();
    size_t len = nondet_size_t();

    __CPROVER_assume(capacity <= size);
    __CPROVER_assume(len <= capacity);

    buf.allocator = NULL;
    buf.buffer = initial_tail;
    buf.len = len;
    buf.capacity = capacity;

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    struct aws_allocator *old_ring_allocator = ring_buffer.allocator;
    uint8_t *old_ring_allocation = ring_buffer.allocation;
    uint8_t *old_ring_allocation_end = ring_buffer.allocation_end;
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    uint8_t *old_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);

    __CPROVER_assume(old_tail == buf.buffer);

    struct aws_byte_buf old_buf = buf;

    struct store_byte_from_buffer old_allocation_byte;
    save_byte_from_array(ring_buffer.allocation, size, &old_allocation_byte);

    aws_ring_buffer_release(&ring_buffer, &buf);

    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);

    assert(new_tail == old_buf.buffer + old_buf.capacity);
    assert(new_head == old_head);

    assert(ring_buffer.allocator == old_ring_allocator);
    assert(ring_buffer.allocation == old_ring_allocation);
    assert(ring_buffer.allocation_end == old_ring_allocation_end);

    assert(buf.allocator == NULL);
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    assert_byte_from_buffer_matches(ring_buffer.allocation, &old_allocation_byte);

    assert(aws_ring_buffer_check_atomic_ptr(&ring_buffer, new_tail));
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));

    aws_ring_buffer_clean_up(&ring_buffer);
}
