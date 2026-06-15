#include <assert.h>
#include <stddef.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0);
    if (aws_ring_buffer_init(&ring_buf, allocator, ring_size) != AWS_OP_SUCCESS) {
        return;
    }

    struct aws_byte_buf buf;
    buf.buffer = NULL;
    buf.capacity = 0;
    buf.len = 0;
    buf.allocator = allocator;
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    size_t acquire_size = nondet_size_t();
    __CPROVER_assume(acquire_size > 0 && acquire_size <= ring_size);
    int acquire_res = aws_ring_buffer_acquire(&ring_buf, acquire_size, &buf);
    __CPROVER_assume(acquire_res == AWS_OP_SUCCESS);
    __CPROVER_assume(!aws_ring_buffer_is_empty(&ring_buf));
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    aws_ring_buffer_release(&ring_buf, &buf);

    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
