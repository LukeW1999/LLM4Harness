#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness(void) {
    /* 1. Declare structures */
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* Allocate and bound the ring buffer's allocation */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);
    ring_buffer.allocation = malloc(ring_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + ring_size;
    ring_buffer.allocator = aws_default_allocator();

    /* Initialize atomic head and tail to some valid pointers within the allocation */
    uint8_t *head_ptr = ring_buffer.allocation;
    uint8_t *tail_ptr = ring_buffer.allocation;
    aws_atomic_store_ptr(&ring_buffer.head, head_ptr);
    aws_atomic_store_ptr(&ring_buffer.tail, tail_ptr);

    /* Assume the ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Set up a byte buffer that belongs to the ring buffer pool */
    size_t buf_capacity = nondet_size_t();
    __CPROVER_assume(buf_capacity > 0);
    __CPROVER_assume(buf_capacity <= ring_size);
    size_t offset = nondet_size_t();
    __CPROVER_assume(offset + buf_capacity <= ring_size);
    buf.buffer = ring_buffer.allocation + offset;
    buf.capacity = buf_capacity;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = aws_default_allocator();

    /* Assume the buffer indeed belongs to the pool */
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    /* 2. Save old state */
    struct aws_ring_buffer old_ring = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 4. Postconditions */

    /* Tail pointer must be updated to point to the end of the released buffer */
    assert(aws_atomic_load_ptr(&ring_buffer.tail) == old_buf.buffer + old_buf.capacity);

    /* The released byte buffer must be zeroed */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* Unchanged fields of the ring buffer */
    assert(ring_buffer.allocator == old_ring.allocator);
    assert(ring_buffer.allocation == old_ring.allocation);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);
    assert(aws_atomic_load_ptr(&ring_buffer.head) == aws_atomic_load_ptr(&old_ring.head));

    /* 5. Validity invariant must still hold */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
