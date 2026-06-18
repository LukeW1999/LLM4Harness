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

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);
    
    struct aws_ring_buffer ring_buffer;
    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    
    struct aws_allocator *old_rb_allocator = ring_buffer.allocator;
    uint8_t *old_rb_allocation = ring_buffer.allocation;
    uint8_t *old_rb_allocation_end = ring_buffer.allocation_end;

    struct aws_allocator *old_buf_allocator = buf.allocator;
    uint8_t *old_buf_buffer = buf.buffer;
    size_t old_buf_len = buf.len;
    size_t old_buf_capacity = buf.capacity;
    
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);
    
    assert(ring_buffer.allocator == old_rb_allocator);
    assert(ring_buffer.allocation == old_rb_allocation);
    assert(ring_buffer.allocation_end == old_rb_allocation_end);

    assert(buf.allocator == old_buf_allocator);
    assert(buf.buffer == old_buf_buffer);
    assert(buf.len == old_buf_len);
    assert(buf.capacity == old_buf_capacity);
    
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));
}
