#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#    define MAX_BUFFER_SIZE 10
#endif

extern size_t nondet_size_t(void);

void aws_ring_buffer_release_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_ring_buffer ring_buffer;

    size_t allocation_size = nondet_size_t();
    __CPROVER_assume(allocation_size > 1);
    __CPROVER_assume(allocation_size <= MAX_BUFFER_SIZE);

    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, allocation_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);

    assert(aws_ring_buffer_is_valid(&ring_buffer));

    struct aws_byte_buf buf;
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size < allocation_size);

    int acquire_result = aws_ring_buffer_acquire(&ring_buffer, requested_size, &buf);
    __CPROVER_assume(acquire_result == AWS_OP_SUCCESS);

    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));

    aws_ring_buffer_release(&ring_buffer, &buf);

    assert(aws_ring_buffer_is_valid(&ring_buffer));

    aws_ring_buffer_clean_up(&ring_buffer);
}
