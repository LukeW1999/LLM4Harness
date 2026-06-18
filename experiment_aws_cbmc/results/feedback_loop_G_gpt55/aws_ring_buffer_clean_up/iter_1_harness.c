#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;

    if (nondet_bool()) {
        size_t size = nondet_size_t();
        __CPROVER_assume(size > 0);
        __CPROVER_assume(size <= MAX_BUFFER_SIZE);

        ring_buf.allocator = aws_default_allocator();
        __CPROVER_assume(ring_buf.allocator != NULL);

        ring_buf.allocation = malloc(size);
        __CPROVER_assume(ring_buf.allocation != NULL);

        ring_buf.allocation_end = ring_buf.allocation + size;
        aws_atomic_init_ptr(&ring_buf.head, ring_buf.allocation);
        aws_atomic_init_ptr(&ring_buf.tail, ring_buf.allocation);
    } else {
        ring_buf.allocator = NULL;
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
        aws_atomic_init_ptr(&ring_buf.head, NULL);
        aws_atomic_init_ptr(&ring_buf.tail, NULL);
    }

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    bool had_allocation = ring_buf.allocation != NULL;

    aws_ring_buffer_clean_up(&ring_buf);

    if (had_allocation) {
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
        assert(ring_buf.allocation_end == NULL);
    } else {
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
        assert(ring_buf.allocation_end == NULL);
    }

    assert(aws_ring_buffer_is_valid(&ring_buf));
}
