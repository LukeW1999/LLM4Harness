#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_ring_buffer_clean_up_harness() {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring_buf;

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

    /* allocator must be a valid allocator */
    ring_buf.allocator = aws_default_allocator();

    /* atomic variables can be nondeterministic; zero-initialized is fine for validity */
    ring_buf.head = (struct aws_atomic_var){0};
    ring_buf.tail = (struct aws_atomic_var){0};

    /* assume the ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save old state */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 4. Postconditions: all fields must be zero after clean up */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.head == (struct aws_atomic_var){0});
    assert(ring_buf.tail == (struct aws_atomic_var){0});

    /* 5. Validity invariant must still hold (zeroed struct is considered valid) */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
