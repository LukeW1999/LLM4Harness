#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    struct aws_ring_buffer ring;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);

    int init_res = aws_ring_buffer_init(&ring, allocator, ring_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    bool expected = false;
    if (buf.buffer != NULL && ring.allocation != NULL && ring.allocation_end != NULL) {
        if (buf.buffer >= ring.allocation &&
            (size_t)(buf.buffer - ring.allocation) + buf.len <=
                (size_t)(ring.allocation_end - ring.allocation)) {
            expected = true;
        }
    }
    assert(result == expected);

    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.head == old_ring.head);
    assert(ring.tail == old_ring.tail);

    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    assert(aws_ring_buffer_is_valid(&ring));
}
