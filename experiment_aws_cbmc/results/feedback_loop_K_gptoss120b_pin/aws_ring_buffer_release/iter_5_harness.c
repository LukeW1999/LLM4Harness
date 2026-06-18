/* CBMC harness for aws_ring_buffer_release */
#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <assert.h>
#include <stddef.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring_buffer;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t init_size = nondet_uint64_t();
    __CPROVER_assume(init_size > 0);
    __CPROVER_assume(init_size <= 1024 * 1024);

    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, init_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    uint8_t *tail_before_acquire = AWS_ATOMIC_LOAD_PTR(&ring_buffer.tail);
    uint8_t *head_before_acquire = AWS_ATOMIC_LOAD_PTR(&ring_buffer.head);

    struct aws_byte_buf buf = {0};
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    size_t acquire_size = nondet_uint64_t();
    __CPROVER_assume(acquire_size > 0);
    __CPROVER_assume(acquire_size <= init_size / 2);

    int acquire_result = aws_ring_buffer_acquire(&ring_buffer, acquire_size, &buf);
    __CPROVER_assume(acquire_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));
    __CPROVER_assume(buf.capacity > 0);
    __CPROVER_assume(buf.buffer != NULL);

    aws_ring_buffer_release(&ring_buffer, &buf);

    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    uint8_t *actual_tail = AWS_ATOMIC_LOAD_PTR(&ring_buffer.tail);

    /* Compute expected tail after release */
    uint8_t *base = ring_buffer.allocation;
    size_t capacity = ring_buffer.allocation_size;
    size_t offset_before = (size_t)(tail_before_acquire - base);
    size_t offset_after = (offset_before + acquire_size) % capacity;
    uint8_t *expected_tail = base + offset_after;

    assert(actual_tail == expected_tail);
    /* head may advance after release, so we do not assert it unchanged */
}
