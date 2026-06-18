#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness(void) {
    /* 1. Declare the ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Allocate a default allocator */
    ring_buf.allocator = aws_default_allocator();

    /* Non‑deterministically decide whether the buffer has an allocation */
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

    /* Initialise atomic head and tail (they are simple structs) */
    ring_buf.head = (struct aws_atomic_var){ .value = 0 };
    ring_buf.tail = (struct aws_atomic_var){ .value = 0 };

    /* Assume the ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save old state */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 4. Post‑conditions: all fields must be zeroed */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.head.value == 0);
    assert(ring_buf.tail.value == 0);

    /* 5. The structure after clean‑up should still satisfy the validity predicate
       (zeroed structures are considered valid for the purpose of this harness) */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
