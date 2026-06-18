#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_acquire_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    size_t capacity;
    __CPROVER_assume(capacity > 0 && capacity <= 1024 * 1024);
    
    struct aws_ring_buffer ring_buf;
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, capacity);
    if (init_result != AWS_OP_SUCCESS) {
        return;
    }
    assert(aws_ring_buffer_is_valid(&ring_buf));

    size_t requested_size;
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size <= capacity);

    struct aws_byte_buf dest = { .buffer = NULL, .len = 0, .capacity = 0 };

    size_t old_head = aws_atomic_load_int(&ring_buf.head);
    size_t old_tail = aws_atomic_load_int(&ring_buf.tail);
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    struct aws_allocator *old_allocator = ring_buf.allocator;

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == requested_size);
        assert(dest.capacity == requested_size);
        assert(dest.buffer != NULL);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        
        assert(aws_atomic_load_int(&ring_buf.head) == old_head);
        assert(aws_atomic_load_int(&ring_buf.tail) == old_tail);
    }

    assert(ring_buf.allocator == old_allocator);
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);

    assert(aws_ring_buffer_is_valid(&ring_buf));
}
