#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

static bool is_power_of_two(size_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}

void aws_ring_buffer_init_harness(void) {
    struct aws_ring_buffer ring_buf;
    /* ensure the structure starts in a known state */
    AWS_ZERO_STRUCT(ring_buf);

    size_t size = nondet_size_t();
    bool want_success = nondet_bool();

    /* bound size to keep the state space finite */
    __CPROVER_assume(size <= 1024);

    if (want_success) {
        /* choose a size that satisfies the implementation’s requirements */
        __CPROVER_assume(is_power_of_two(size));
        __CPROVER_assume(size > 0);
    } else {
        /* choose a size that will cause initialization to fail */
        __CPROVER_assume(!is_power_of_two(size) || size == 0);
    }

    struct aws_allocator *allocator = aws_default_allocator();

    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    if (result == AWS_OP_SUCCESS) {
        /* allocation must be non‑NULL */
        assert(ring_buf.allocation != NULL);
        /* allocator must be the one we passed */
        assert(ring_buf.allocator == allocator);
        /* head and tail must point to the start of the allocation */
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
        /* allocation_end must be exactly allocation + size */
        assert(ring_buf.allocation_end == (uint8_t *)ring_buf.allocation + size);
    } else {
        /* on failure the ring buffer must be left empty */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
        assert(ring_buf.allocation_end == NULL);
    }

    /* validity invariant: the ring buffer is valid iff the call succeeded */
    assert(aws_ring_buffer_is_valid(&ring_buf) == (result == AWS_OP_SUCCESS));
}
