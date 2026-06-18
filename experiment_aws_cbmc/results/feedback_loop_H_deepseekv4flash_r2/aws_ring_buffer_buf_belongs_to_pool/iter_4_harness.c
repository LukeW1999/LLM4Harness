#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* allocate non-deterministic memory for ring buffer */
    size_t alloc_size;
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_BUFFER_SIZE);
    ring_buffer.allocator = aws_default_allocator();
    ring_buffer.allocation = (uint8_t *)malloc(alloc_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + alloc_size;

    /* set up byte buffer */
    size_t capacity;
    __CPROVER_assume(capacity > 0 && capacity <= MAX_BUFFER_SIZE);
    buf.capacity = capacity;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = aws_default_allocator();

    /* decide non-deterministically whether buf.buffer lies inside the ring buffer allocation */
    bool inside;
    if (inside) {
        size_t offset;
        __CPROVER_assume(offset < alloc_size);
        buf.buffer = ring_buffer.allocation + offset;
    } else {
        buf.buffer = (uint8_t *)malloc(buf.capacity);
        __CPROVER_assume(buf.buffer != NULL);
    }

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);

    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);

    assert(aws_byte_buf_is_valid(&buf));

    bool buf_in_range = (buf.buffer >= ring_buffer.allocation && buf.buffer < ring_buffer.allocation_end);
    assert(result == buf_in_range);
}
