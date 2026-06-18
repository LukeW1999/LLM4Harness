#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_release_harness() {
    /* Allocate a fixed-size storage for the ring buffer */
    uint8_t storage[256];
    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize the ring buffer */
    struct aws_ring_buffer ring_buffer;
    ring_buffer.allocator = allocator;
    ring_buffer.allocation = storage;
    ring_buffer.allocation_end = storage + sizeof(storage);
    ring_buffer.head = storage + 16;               /* non‑empty buffer */
    aws_atomic_init_ptr(&ring_buffer.tail, storage);
    ring_buffer.size = 16;                         /* bytes currently in buffer */
    ring_buffer.capacity = sizeof(storage);

    /* Prepare a byte buffer that points to the current tail */
    struct aws_byte_buf buf;
    buf.buffer = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    buf.capacity = 16;               /* deterministic size within the buffer */
    buf.len = buf.capacity;
    buf.allocator = allocator;

    /* Save old state */
    struct aws_ring_buffer old_ring = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Postconditions */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    assert(ring_buffer.allocator == old_ring.allocator);
    assert(ring_buffer.allocation == old_ring.allocation);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);

    assert(aws_atomic_load_ptr(&ring_buffer.tail) == (void *)(old_buf.buffer + old_buf.capacity));

    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
