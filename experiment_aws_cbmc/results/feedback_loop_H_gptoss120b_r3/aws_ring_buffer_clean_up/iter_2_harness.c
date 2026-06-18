#include <assert.h>
#include <stdlib.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;

    /* allocator must be a valid allocator */
    struct aws_allocator *alloc = aws_default_allocator();
    ring_buf.allocator = alloc;

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

    /* initialize atomic indices to zero */
    aws_atomic_store_int(&ring_buf.head, 0);
    aws_atomic_store_int(&ring_buf.tail, 0);

    /* assume the ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* postconditions */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.allocator == alloc);
    assert(aws_atomic_load_int(&ring_buf.head) == 0);
    assert(aws_atomic_load_int(&ring_buf.tail) == 0);
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
