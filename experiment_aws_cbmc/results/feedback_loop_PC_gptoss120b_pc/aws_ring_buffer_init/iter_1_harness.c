#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_RING_BUFFER_SIZE 1024

void aws_ring_buffer_init_harness(void) {
    /* 1. Declare data structures */
    struct aws_ring_buffer ring_buf;
    struct aws_ring_buffer old;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= MAX_RING_BUFFER_SIZE);

    /* Save old state before the call */
    old = ring_buf;

    /* 2. Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 3. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Changed fields */
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation_end == (uint8_t *)ring_buf.allocation + size);
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
    } else {
        /* Failure path – struct should be zeroed */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.head) == NULL);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == NULL);
    }

    /* 4. Invariant: ring buffer must always be valid */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
