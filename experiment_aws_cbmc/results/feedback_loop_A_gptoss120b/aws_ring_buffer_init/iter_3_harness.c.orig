#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>
#include <stdlib.h>
#include <string.h>
#include "cbmc_proof.h"

void aws_ring_buffer_init_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator;
    size_t size;

    /* nondet choices that make both success and failure reachable */
    bool allocator_is_null = nondet_bool();
    bool size_is_zero = nondet_bool();

    if (allocator_is_null) {
        allocator = NULL;
    } else {
        allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
        __CPROVER_assume(allocator != NULL);
    }

    size = nondet_size_t();
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);
    if (size_is_zero) {
        size = 0;
    }

    /* start with a clean object */
    memset(&ring_buf, 0, sizeof(ring_buf));

    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* unconditional post‑conditions that must always hold */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    if (result == AWS_OP_SUCCESS) {
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation_end == (uint8_t *)ring_buf.allocation + size);
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
    } else {
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
    }
}
