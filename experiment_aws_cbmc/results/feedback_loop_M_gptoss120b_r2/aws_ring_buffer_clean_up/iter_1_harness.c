#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness() {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring_buf;
    /* Use the default allocator */
    ring_buf.allocator = aws_default_allocator();

    /* Non-deterministically decide whether the buffer has an allocation */
    bool has_allocation = nondet_bool();

    if (has_allocation) {
        size_t alloc_size = nondet_size_t();
        __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_BUFFER_SIZE);
        ring_buf.allocation = (uint8_t *)malloc(alloc_size);
        __CPROVER_assume(ring_buf.allocation != NULL);
        ring_buf.allocation_end = ring_buf.allocation + alloc_size;
    } else {
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
    }

    /* Initialize atomic head and tail to zero */
    ring_buf.head = (struct aws_atomic_var){0};
    ring_buf.tail = (struct aws_atomic_var){0};

    /* Assume the ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save old state before calling */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 4. Assert postconditions */

    /* All fields should be zeroed after cleanup */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.head.value == 0);
    assert(ring_buf.tail.value == 0);

    /* 5. No other fields exist, so nothing else to assert unchanged */

    /* 6. The ring buffer should still satisfy its validity predicate (zeroed struct is considered valid) */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
