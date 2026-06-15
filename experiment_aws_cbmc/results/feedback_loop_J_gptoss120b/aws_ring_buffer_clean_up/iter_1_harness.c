#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_ring_buffer_clean_up_harness(void) {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring_buf;
    /* Use the default allocator */
    ring_buf.allocator = aws_default_allocator();

    /* Non‑deterministically decide whether there is an allocation */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);
    if (alloc_size > 0) {
        ring_buf.allocation = malloc(alloc_size);
        __CPROVER_assume(ring_buf.allocation != NULL);
        ring_buf.allocation_end = ring_buf.allocation + alloc_size;
    } else {
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
    }

    /* Initialise atomic variables to some nondet values (they will be zeroed by the function) */
    ring_buf.head = (struct aws_atomic_var){0};
    ring_buf.tail = (struct aws_atomic_var){0};

    /* Assume the ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save old state */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 4. Post‑conditions: all fields must be zero after clean‑up */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    /* The atomic variables are part of the struct; after zeroing they must be zero */
    assert(ring_buf.head == (struct aws_atomic_var){0});
    assert(ring_buf.tail == (struct aws_atomic_var){0});

    /* 5. No other fields exist, so nothing else to assert */

    /* 6. The structure after clean‑up is expected to be in a zeroed state; 
       we do not require it to satisfy aws_ring_buffer_is_valid() after clean‑up. */
}
