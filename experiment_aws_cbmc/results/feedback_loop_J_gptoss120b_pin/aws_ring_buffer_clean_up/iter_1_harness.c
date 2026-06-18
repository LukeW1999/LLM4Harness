#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_ring_buffer_clean_up_harness(void) {
    /* 1. Declare the ring buffer and set up nondeterministic initial state */
    struct aws_ring_buffer ring_buf;
    /* Use the default allocator as required */
    ring_buf.allocator = aws_default_allocator();

    /* Nondeterministically decide whether the allocation is present */
    if (nondet_bool()) {
        /* Allocate a minimal non‑zero sized buffer */
        ring_buf.allocation = (uint8_t *)malloc(1);
        ring_buf.allocation_end = ring_buf.allocation + 1;
    } else {
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
    }

    /* Initialize atomic variables with nondeterministic values */
    ring_buf.head = (struct aws_atomic_var){ .value = nondet_size_t() };
    ring_buf.tail = (struct aws_atomic_var){ .value = nondet_size_t() };

    /* Assume the ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save old state before the call */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 4. Post‑conditions: all fields must be zeroed */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    /* The atomic variables are part of the struct and must be zeroed as well */
    assert(ring_buf.head.value == 0);
    assert(ring_buf.tail.value == 0);

    /* 5. The ring buffer is no longer valid after clean‑up */
    assert(!aws_ring_buffer_is_valid(&ring_buf));
}
