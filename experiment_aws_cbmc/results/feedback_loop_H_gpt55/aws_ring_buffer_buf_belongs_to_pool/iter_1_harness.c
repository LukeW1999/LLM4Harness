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

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    struct aws_ring_buffer ring_buffer;

    size_t allocation_size = nondet_size_t();
    __CPROVER_assume(allocation_size > 0);
    __CPROVER_assume(allocation_size <= MAX_BUFFER_SIZE);

    ring_buffer.allocator = aws_default_allocator();
    ring_buffer.allocation = malloc(allocation_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + allocation_size;

    size_t head_offset = nondet_size_t();
    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(head_offset < allocation_size);
    __CPROVER_assume(tail_offset < allocation_size);

    aws_atomic_init_ptr(&ring_buffer.head, ring_buffer.allocation + head_offset);
    aws_atomic_init_ptr(&ring_buffer.tail, ring_buffer.allocation + tail_offset);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    struct aws_byte_buf buf;
    size_t buf_offset = nondet_size_t();
    __CPROVER_assume(buf_offset <= allocation_size);

    buf.allocator = NULL;
    buf.buffer = ring_buffer.allocation + buf_offset;

    size_t buf_capacity = nondet_size_t();
    __CPROVER_assume(buf_capacity <= allocation_size - buf_offset);
    buf.capacity = buf_capacity;

    size_t buf_len = nondet_size_t();
    __CPROVER_assume(buf_len <= buf.capacity);
    buf.len = buf_len;

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;
    void *old_head = aws_atomic_load_ptr(&ring_buffer.head);
    void *old_tail = aws_atomic_load_ptr(&ring_buffer.tail);

    struct store_byte_from_buffer old_allocation_byte;
    save_byte_from_array(ring_buffer.allocation, allocation_size, &old_allocation_byte);

    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    if (result) {
        assert(buf.buffer >= ring_buffer.allocation);
        assert(buf.buffer < ring_buffer.allocation_end);
        assert(buf.buffer + buf.capacity <= ring_buffer.allocation_end);
    } else {
        assert(!(buf.buffer >= ring_buffer.allocation && buf.buffer < ring_buffer.allocation_end));
    }

    assert(result == (buf.buffer >= ring_buffer.allocation && buf.buffer < ring_buffer.allocation_end));

    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    assert(aws_atomic_load_ptr(&ring_buffer.head) == old_head);
    assert(aws_atomic_load_ptr(&ring_buffer.tail) == old_tail);

    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);

    assert_byte_from_buffer_matches(ring_buffer.allocation, &old_allocation_byte);

    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));
}
