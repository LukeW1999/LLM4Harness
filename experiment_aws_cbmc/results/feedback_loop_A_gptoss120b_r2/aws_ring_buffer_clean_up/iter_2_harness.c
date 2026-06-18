#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness(void) {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring_buf;
    /* allocator must be a valid allocator */
    ring_buf.allocator = aws_default_allocator();

    /* nondeterministically decide whether the allocation is present */
    if (nondet_bool()) {
        size_t size = nondet_size_t();
        __CPROVER_assume(size <= 1024);
        ring_buf.allocation = malloc(size);
        __CPROVER_assume(ring_buf.allocation != NULL);
        ring_buf.allocation_end = (uint8_t *)ring_buf.allocation + size;
    } else {
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
    }

    /* initialize atomic variables to nondeterministic values */
    ring_buf.head = (struct aws_atomic_var){ .value = nondet_uint64_t() };
    ring_buf.tail = (struct aws_atomic_var){ .value = nondet_uint64_t() };

    /* assume the structure is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save old state */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 4. Post‑conditions */

    /* All fields must be zeroed after clean‑up */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    /* atomic variables are part of the zeroed struct; we check their raw value */
    assert(ring_buf.head.value == 0);
    assert(ring_buf.tail.value == 0);
}
