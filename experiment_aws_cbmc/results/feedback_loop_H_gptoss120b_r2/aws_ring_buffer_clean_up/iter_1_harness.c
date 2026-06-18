#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness() {
    /* 1. Declare and initialize a ring buffer */
    struct aws_ring_buffer ring_buf;

    /* allocator must be a valid default allocator */
    ring_buf.allocator = aws_default_allocator();

    /* nondeterministically decide whether the buffer has an allocation */
    if (nondet_bool()) {
        size_t alloc_size = nondet_size_t();
        __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);
        ring_buf.allocation = malloc(alloc_size);
        __CPROVER_assume(ring_buf.allocation != NULL);
        ring_buf.allocation_end = (uint8_t *)ring_buf.allocation + alloc_size;
    } else {
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
    }

    /* nondeterministically initialize atomic head/tail values */
    ring_buf.head.value = nondet_size_t();
    ring_buf.tail.value = nondet_size_t();

    /* assume the ring buffer is valid before the call */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save old state */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 4. Postconditions – all fields must be zeroed */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.head.value == 0);
    assert(ring_buf.tail.value == 0);

    /* 5. Validity invariant must still hold after clean‑up */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
