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
    struct aws_ring_buffer ring_buffer = {0};
    struct aws_allocator *allocator = aws_default_allocator();

    uint8_t allocation[MAX_BUFFER_SIZE];

    size_t allocation_size = nondet_size_t();
    __CPROVER_assume(allocation_size > 1);
    __CPROVER_assume(allocation_size <= MAX_BUFFER_SIZE);

    size_t head_offset = nondet_size_t();
    __CPROVER_assume(head_offset < allocation_size);

    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(tail_offset < allocation_size);
    __CPROVER_assume(tail_offset != head_offset);

    size_t buf_capacity = nondet_size_t();
    __CPROVER_assume(buf_capacity > 0);

    if (tail_offset < head_offset) {
        __CPROVER_assume(buf_capacity <= head_offset - tail_offset);
    } else {
        __CPROVER_assume(buf_capacity == allocation_size - tail_offset);
    }

    ring_buffer.allocator = allocator;
    ring_buffer.allocation = allocation;
    ring_buffer.allocation_end = allocation + allocation_size;

    aws_atomic_init_ptr(&ring_buffer.head, ring_buffer.allocation + head_offset);
    aws_atomic_init_ptr(&ring_buffer.tail, ring_buffer.allocation + tail_offset);

    struct aws_byte_buf buf = {0}
