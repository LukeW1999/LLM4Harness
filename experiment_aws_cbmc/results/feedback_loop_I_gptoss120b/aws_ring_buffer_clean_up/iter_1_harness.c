#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness() {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Allocate a non‑deterministic buffer for the ring buffer allocation */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);
    ring_buf.allocation = (uint8_t *)malloc(alloc_size);
    if (ring_buf.allocation) {
        ring_buf.allocation_end = ring_buf.allocation + alloc_size;
    } else {
        ring_buf.allocation_end = NULL;
    }

    /* Use the default allocator */
    ring_buf.allocator = aws_default_allocator();

    /* Initialise atomic variables with nondeterministic values */
    ring_buf.head = (struct aws_atomic_var){ .value = (int)nondet_uint32_t() };
    ring_buf.tail = (struct aws_atomic_var){ .value = (int)nondet_uint32_t() };

    /* Assume the ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save old state */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 4. Post‑conditions: all fields must be zero after clean‑up */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.allocator == NULL);
    assert(aws_atomic_load_int(&ring_buf.head) == 0);
    assert(aws_atomic_load_int(&ring_buf.tail) == 0);

    /* 5. The structure must remain valid (zeroed ring buffer is a valid empty buffer) */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
