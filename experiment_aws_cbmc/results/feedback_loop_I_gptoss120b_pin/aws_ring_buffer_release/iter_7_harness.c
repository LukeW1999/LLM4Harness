#include <assert.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness() {
    /* Allocate a fixed-size storage for the ring buffer */
    uint8_t storage[256];
    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize the ring buffer */
    struct aws_ring_buffer ring_buffer;
    ring_buffer.allocator = allocator;
    ring_buffer.allocation = storage;
    ring_buffer.allocation_end = storage + sizeof(storage);
    aws_atomic_store_ptr(&ring_buffer.head, storage + 16);   /* non‑empty buffer */
    aws_atomic_store_ptr(&ring_buffer.tail, storage);
    ring_buffer.size = 16;                                 /* bytes currently in buffer */
    ring_buffer.capacity = sizeof(storage);

    /* Prepare a byte buffer that points to the current tail */
    struct aws_byte_buf buf;
    buf.buffer = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    buf.capacity = 16;               /* deterministic size within the buffer */
    buf.len = buf.capacity;
    buf.allocator = allocator;

    /* Save old state */
    struct aws_allocator *old_allocator = ring_buffer.allocator;
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;
    uint8_t *old_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    uint8_t *old_buf_buffer = buf.buffer;
    size_t old_buf_capacity = buf.capacity;

    /* Call the function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Postconditions */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    assert(ring_buffer.allocator == old_allocator);
    assert(ring_buffer.allocation == old_allocation);
    assert(ring_buffer.allocation_end == old_allocation_end);

    assert(aws_atomic_load_ptr(&ring_buffer.tail) == (void *)(old_buf_buffer + old_buf_capacity));

    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
