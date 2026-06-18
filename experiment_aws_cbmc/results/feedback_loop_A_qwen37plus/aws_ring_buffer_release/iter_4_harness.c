#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring_buffer;
    ensure_ring_buffer_is_valid(&ring_buffer);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    struct aws_byte_buf buf;
    ensure_byte_buf_is_valid(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    
    __CPROVER_assume(buf.buffer != NULL);
    __CPROVER_assume(buf.buffer >= ring_buffer.allocation);
    __CPROVER_assume(buf.buffer <= ring_buffer.allocation_end);
    __CPROVER_assume(buf.capacity <= (size_t)(ring_buffer.allocation_end - buf.buffer));

    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    aws_ring_buffer_release(&ring_buffer, &buf);

    assert(aws_atomic_load_ptr(&ring_buffer.tail) == old_buf.buffer + old_buf.capacity);

    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    assert(aws_atomic_load_ptr(&ring_buffer.head) == aws_atomic_load_ptr(&old_ring_buffer.head));

    assert(buf.allocator == NULL);
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);

    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
