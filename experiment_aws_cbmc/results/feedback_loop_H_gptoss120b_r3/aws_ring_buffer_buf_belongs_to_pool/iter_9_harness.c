#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    struct aws_ring_buffer ring = {0};
    struct aws_byte_buf buf = {0};

    struct aws_allocator *allocator = aws_default_allocator();
    ring.allocator = allocator;

    size_t alloc_size;
    __CPROVER_assume(alloc_size > 0);
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);
    ring.allocation = (uint8_t *)malloc(alloc_size);
    __CPROVER_assume(ring.allocation != NULL);
    ring.allocation_end = ring.allocation + alloc_size;
    ring.size = alloc_size;

    ring.head.value = 0;
    ring.tail.value = 0;

    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    if (result) {
        assert(buf.buffer >= ring.allocation);
        assert(buf.buffer < ring.allocation_end);
    } else {
        assert(!(buf.buffer >= ring.allocation && buf.buffer < ring.allocation_end));
    }

    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.head.value == old_ring.head.value);
    assert(ring.tail.value == old_ring.tail.value);
    assert(ring.size == old_ring.size);

    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.len <= buf.capacity);

    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
