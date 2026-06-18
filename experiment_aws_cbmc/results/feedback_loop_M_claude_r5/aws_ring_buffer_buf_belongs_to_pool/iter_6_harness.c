#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* 1. Set up the ring buffer */
    struct aws_ring_buffer ring_buffer;
    
    /* Allocate a backing buffer for the ring buffer */
    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);
    
    /* Initialize ring buffer fields non-deterministically but validly */
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buffer.allocator = allocator;
    ring_buffer.allocation = (uint8_t *)malloc(ring_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + ring_size;
    
    /* Set head and tail atomics to valid positions within the buffer */
    size_t head_offset;
    __CPROVER_assume(head_offset <= ring_size);
    size_t tail_offset;
    __CPROVER_assume(tail_offset <= ring_size);
    
    aws_atomic_init_ptr(&ring_buffer.head, (void *)(ring_buffer.allocation + head_offset));
    aws_atomic_init_ptr(&ring_buffer.tail, (void *)(ring_buffer.allocation + tail_offset));
    
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    
    /* 2. Set up the byte buf */
    struct aws_byte_buf buf;
    buf.allocator = allocator;
    buf.len = 0;
    buf.capacity = ring_size;
    
    /* Non-deterministically place buf.buffer within or outside the ring buffer */
    uint8_t *buf_ptr;
    buf.buffer = buf_ptr;
    
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    
    /* 3. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);
    
    /* 4. Assert postconditions */
    if (result) {
        __CPROVER_assert(buf.buffer != NULL, "buf.buffer is not NULL when result is true");
        __CPROVER_assert(buf.buffer >= ring_buffer.allocation, "buf.buffer >= allocation");
        __CPROVER_assert(buf.buffer < ring_buffer.allocation_end, "buf.buffer < allocation_end");
    }
    
    /* Validity invariants must still hold */
    __CPROVER_assert(aws_ring_buffer_is_valid(&ring_buffer), "ring_buffer is valid after call");
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "buf is valid after call");
}
