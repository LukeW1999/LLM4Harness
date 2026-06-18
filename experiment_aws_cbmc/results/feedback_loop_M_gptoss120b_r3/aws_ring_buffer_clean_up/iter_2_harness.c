#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);

    uint8_t *allocation = NULL;
    if (alloc_size > 0) {
        allocation = malloc(alloc_size);
        __CPROVER_assume(allocation != NULL);
    }

    ring_buf.allocation = allocation;
    ring_buf.allocation_end = (allocation != NULL) ? allocation + alloc_size : NULL;
    ring_buf.allocator = aws_default_allocator();

    ring_buf.head = (struct aws_atomic_var){ .value = nondet_uint64_t() };
    ring_buf.tail = (struct aws_atomic_var){ .value = nondet_uint64_t() };

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_ring_buffer old = ring_buf;

    aws_ring_buffer_clean_up(&ring_buf);

    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.head.value == 0);
    assert(ring_buf.tail.value == 0);
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
