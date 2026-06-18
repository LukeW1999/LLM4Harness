#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_release_harness(void) {
    /* Allocate a storage area for the ring buffer */
    uint8_t storage[MAX_BUFFER_SIZE];
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize the ring buffer */
    struct aws_ring_buffer ring_buffer;
    aws_ring_buffer_init(&ring_buffer, allocator, storage, alloc_size);

    /* Set up a byte buffer that belongs to the ring buffer pool */
    size_t buf_capacity = nondet_size_t();
    __CPROVER_assume(buf_capacity <= alloc_size);
    size_t offset = nondet_size_t();
    __CPROVER_assume(offset + buf_capacity <= alloc_size);

    struct aws_byte_buf buf;
    buf.buffer = storage + offset;
    buf.capacity = buf_capacity;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = allocator;

    /* Assume the ring buffer is valid before the call */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Save old state */
    struct aws_ring_buffer old_ring = ring_buffer;
    struct aws_byte_buf old_buf = buf;
    size_t old_head = aws_atomic_load_int(&ring_buffer.head);
    size_t old_tail = aws_atomic_load_int(&ring_buffer.tail);

    /* Call the function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Post‑conditions */

    /* Tail pointer must be updated to point just past the released region */
    assert(aws_atomic_load_int(&ring_buffer.tail) == old_tail + old_buf.capacity);

    /* All other ring buffer fields must remain unchanged */
    assert(aws_atomic_load_int(&ring_buffer.head) == old_head);
    assert(ring_buffer.allocator == old_ring.allocator);
    assert(ring_buffer.allocation == old_ring.allocation);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);

    /* The released byte buffer must be zeroed */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* Validity invariants must still hold */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
