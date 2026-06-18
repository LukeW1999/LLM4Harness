#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE (1024u)
#endif

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;

    bool has_allocation = aws_nondet_bool();
    if (has_allocation) {
        size_t size = aws_nondet_size_t();
        __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);
        ring_buf.allocator = aws_default_allocator();
        ring_buf.allocation = (uint8_t *)malloc(size);
        __CPROVER_assume(ring_buf.allocation != NULL);
        ring_buf.allocation_end = ring_buf.allocation + size;
        size_t head = aws_nondet_size_t();
        size_t tail = aws_nondet_size_t();
        __CPROVER_assume(head < size);
        __CPROVER_assume(tail < size);
        aws_atomic_store_int(&ring_buf.head, head);
        aws_atomic_store_int(&ring_buf.tail, tail);
    } else {
        ring_buf.allocator = aws_default_allocator();
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
        aws_atomic_store_int(&ring_buf.head, 0);
        aws_atomic_store_int(&ring_buf.tail, 0);
    }

    /* Only test valid ring buffers, as required by the function contract */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    aws_ring_buffer_clean_up(&ring_buf);

    /* After cleanup, the entire struct must be zeroed. */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(aws_atomic_load_int(&ring_buf.head) == 0);
    assert(aws_atomic_load_int(&ring_buf.tail) == 0);
}
