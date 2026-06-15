#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_ALLOC_SIZE 256
#define MAX_BYTE_BUF_SIZE 128

void aws_ring_buffer_release_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    size_t allocation_size = nondet_size_t();
    __CPROVER_assume(allocation_size > 0 && allocation_size <= MAX_ALLOC_SIZE);
    uint8_t *allocation = malloc(allocation_size);
    __CPROVER_assume(allocation != NULL);

    struct aws_ring_buffer ring;
    aws_ring_buffer_init(&ring, alloc, allocation, allocation_size);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    bool valid_release = nondet_bool();

    struct aws_byte_buf buf;
    if (valid_release) {
        size_t sz = nondet_size_t();
        __CPROVER_assume(sz <= allocation_size);
        int ac_ret = aws_ring_buffer_acquire(&ring, &buf, sz);
        __CPROVER_assume(ac_ret == 0);
    } else {
        size_t cap = nondet_size_t();
        __CPROVER_assume(cap > 0 && cap <= MAX_BYTE_BUF_SIZE);
        uint8_t *buf_mem = malloc(cap);
        __CPROVER_assume(buf_mem != NULL);
        buf.buffer = buf_mem;
        buf.capacity = cap;
        buf.len = 0;
        buf.allocator = NULL;
        __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BYTE_BUF_SIZE));
    }

    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    int ret = aws_ring_buffer_release(&ring, &buf);

    if (ret == 0) {
        assert(buf.buffer == NULL);
        assert(buf.capacity == 0);
        assert(buf.len == 0);
        assert(buf.allocator == NULL);
        assert((uint8_t *)ring.tail.value == (uint8_t *)old_ring.tail.value + old_buf.capacity);
    } else {
        assert(ring.tail.value == old_ring.tail.value);
        assert(ring.head.value == old_ring.head.value);
        assert(ring.allocator == old_ring.allocator);
        assert(ring.allocation == old_ring.allocation);
        assert(ring.allocation_end == old_ring.allocation_end);
        assert(buf.buffer == old_buf.buffer);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.len == old_buf.len);
        assert(buf.allocator == old_buf.allocator);
    }

    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BYTE_BUF_SIZE));

    aws_ring_buffer_clean_up(&ring);
    free(allocation);
    if (!valid_release) {
        free(buf.buffer);
    }
}
