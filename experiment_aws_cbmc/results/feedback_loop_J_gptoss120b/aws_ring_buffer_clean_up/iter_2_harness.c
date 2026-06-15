#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_ALLOC_SIZE 1024U

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    ring_buf.allocator = aws_default_allocator();

    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size <= MAX_ALLOC_SIZE);
    if (alloc_size > 0) {
        ring_buf.allocation = malloc(alloc_size);
        __CPROVER_assume(ring_buf.allocation != NULL);
        ring_buf.allocation_end = (uint8_t *)ring_buf.allocation + alloc_size;
    } else {
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
    }

    ring_buf.head = (struct aws_atomic_var){0};
    ring_buf.tail = (struct aws_atomic_var){0};

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_ring_buffer old = ring_buf;

    aws_ring_buffer_clean_up(&ring_buf);

    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.head == (struct aws_atomic_var){0});
    assert(ring_buf.tail == (struct aws_atomic_var){0});
}
