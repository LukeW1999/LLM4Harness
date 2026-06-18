#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

void aws_ring_buffer_release_harness() {
    struct aws_ring_buffer ring_buffer;
    make_aws_ring_buffer_data(&ring_buffer);
    ring_buffer.allocator = aws_default_allocator();
    
    if (ring_buffer.allocation == NULL || ring_buffer.allocation_end == NULL || ring_buffer.allocation >= ring_buffer.allocation_end) {
        size_t capacity = 10;
        ring_buffer.allocation = malloc(capacity);
        ring_buffer.allocation_end = ring_buffer.allocation + capacity;
    }
    aws_atomic_store_ptr(&ring_buffer.head, ring_buffer.allocation);
    aws_atomic_store_ptr(&ring_buffer.tail, ring_buffer.allocation);
    
    struct aws_byte_buf buf;
    buf.allocator = ring_buffer.allocator;
    buf.len = 0;
    
    size_t offset;
    __CPROVER_assume(offset < (size_t)(ring_buffer.allocation_end - ring_buffer.allocation));
    buf.buffer = ring_buffer.allocation + offset;
    
    size_t buf_capacity;
    __CPROVER_assume(buf_capacity > 0 && buf_capacity <= (size_t)(ring_buffer.allocation_end - buf.buffer));
    buf.capacity = buf_capacity;
    
    /* Save old state */
    void *old_tail = aws_atomic_load_ptr(&ring_buffer.tail);
    void *old_head = aws_atomic_load_ptr(&ring_buffer.head);
    struct aws_allocator *old_allocator = ring_buffer.allocator;
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;
    struct aws_byte_buf old_buf = buf;
    
    /* Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);
    
    /* Assert changed fields */
    assert(aws_atomic_load_ptr(&ring_buffer.tail) == (void *)(old_buf.buffer + old_buf.capacity));
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    
    /* Assert unchanged fields */
    assert(ring_buffer.allocator == old_allocator);
    assert(ring_buffer.allocation == old_allocation);
    assert(ring_buffer.allocation_end == old_allocation_end);
    assert(aws_atomic_load_ptr(&ring_buffer.head) == old_head);
    
    /* Assert validity invariants */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));
}
