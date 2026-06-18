#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buffer;
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    __CPROVER_assume(ring_buffer.allocation == NULL || 
                     (size_t)(ring_buffer.allocation_end - ring_buffer.allocation) <= MAX_BUFFER_SIZE);

    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(buf.buffer == NULL || 
                     (buf.buffer >= ring_buffer.allocation && 
                      buf.buffer + buf.capacity <= ring_buffer.allocation_end));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 4. Assert postconditions for changed fields */
    assert(aws_atomic_load_ptr(&ring_buffer.tail) == old_buf.buffer + old_buf.capacity);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    assert(aws_atomic_load_ptr(&ring_buffer.head) == aws_atomic_load_ptr(&old_ring_buffer.head));

    /* buf is zeroed out by AWS_ZERO_STRUCT */
    assert(buf.allocator == NULL);
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);

    /* 6. Assert validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
