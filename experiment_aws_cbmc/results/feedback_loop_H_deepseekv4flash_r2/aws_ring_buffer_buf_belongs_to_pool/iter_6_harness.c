#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1000

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    struct aws_allocator *allocator = aws_default_allocator();

    // Initialize ring buffer with nondeterministic size
    size_t alloc_size;
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_BUFFER_SIZE);
    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, alloc_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);

    // Nondeterministically decide whether buf.buffer lies inside the ring buffer allocation
    bool inside;
    if (inside) {
        // Set buf to point inside the ring buffer
        size_t offset;
        __CPROVER_assume(offset < alloc_size);
        buf.buffer = ring_buffer.allocation + offset;
        buf.capacity = nondet_size_t();
        __CPROVER_assume(buf.capacity > 0 && buf.capacity <= MAX_BUFFER_SIZE);
        buf.len = nondet_size_t();
        __CPROVER_assume(buf.len <= buf.capacity);
        buf.allocator = allocator;
    } else {
        // Initialize a separate byte buffer
        size_t capacity;
        __CPROVER_assume(capacity > 0 && capacity <= MAX_BUFFER_SIZE);
        int init_result_buf = aws_byte_buf_init(&buf, allocator, capacity);
        __CPROVER_assume(init_result_buf == AWS_OP_SUCCESS);
        buf.len = nondet_size_t();
        __CPROVER_assume(buf.len <= buf.capacity);
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
