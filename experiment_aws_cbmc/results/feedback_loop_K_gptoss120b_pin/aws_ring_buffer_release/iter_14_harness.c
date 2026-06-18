#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/error.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring_buffer;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t init_size;
    __CPROVER_assume(init_size > 0);
    __CPROVER_assume(init_size <= 1024 * 1024);

    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, init_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);

    size_t tail_offset_before = ring_buffer.tail;
    size_t head_offset_before = ring_buffer.head;

    struct aws_byte_buf buf = {0};

    size_t acquire_size;
    __CPROVER_assume(acquire_size > 0);
    __CPROVER_assume(acquire_size <= init_size / 2);

    int acquire_result = aws_ring_buffer_acquire(&ring_buffer, acquire_size, &buf);
    __CPROVER_assume(acquire_result == AWS_OP_SUCCESS);
    __CPROVER_assume(buf.capacity > 0);
    __CPROVER_assume(buf.buffer != NULL);

    aws_ring_buffer_release(&ring_buffer, &buf);

    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    size_t tail_offset_after = ring_buffer.tail;

    uint8_t *base = ring_buffer.allocation;
    size_t capacity = ring_buffer.allocation_size;
    size_t offset_before = tail_offset_before;
    size_t offset_after = (offset_before + acquire_size) % capacity;
    uint8_t *expected_tail = base + offset_after;
    uint8_t *actual_tail = base + tail_offset_after;

    assert(actual_tail == expected_tail);
}
