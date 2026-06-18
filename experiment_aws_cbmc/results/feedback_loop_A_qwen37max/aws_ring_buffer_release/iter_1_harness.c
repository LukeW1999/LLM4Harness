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

void aws_ring_buffer_release_harness() {
    struct aws_ring_buffer ring_buffer;
    ring_buffer.allocator = aws_default_allocator();
    
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);
    ring_buffer.allocation = malloc(size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + size;
    
    aws_atomic_store_ptr(&ring_buffer.head, ring_buffer.allocation);
    aws_atomic_store_ptr(&ring_buffer.tail, ring_buffer.allocation);
    
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    
    /* Ensure buf belongs to the ring buffer's allocation */
    __CPROVER_assume(buf.buffer >= ring_buffer.allocation);
    __CPROVER_assume(buf.buffer + buf.capacity <= ring_buffer.allocation_end);

    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Postconditions for buf (zeroed out by AWS_ZERO_STRUCT) */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* Postconditions for ring_buffer tail update */
    void *tail_ptr = aws_atomic_load_ptr(&ring_buffer.tail);
    assert(tail_ptr == (void *)(old_buf.buffer + old_buf.capacity));

    /* Postconditions for ring_buffer unchanged fields */
    void *head_ptr = aws_atomic_load_ptr(&ring_buffer.head);
    void *old_head_ptr = aws_atomic_load_ptr(&old_ring_buffer.head);
    assert(head_ptr == old_head_ptr);

    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);

    /* Validity invariant */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
