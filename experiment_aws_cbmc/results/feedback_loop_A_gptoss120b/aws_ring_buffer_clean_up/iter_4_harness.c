#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024U

/* Nondeterministically allocate a buffer of bounded size */
static uint8_t *nondet_allocation(size_t *out_size) {
    size_t sz = nondet_size_t();
    __CPROVER_assume(sz <= MAX_BUFFER_SIZE);
    *out_size = sz;
    return sz ? malloc(sz) : NULL;
}

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    size_t alloc_sz;

    /* Allocate (or not) the underlying buffer */
    ring_buf.allocation = nondet_allocation(&alloc_sz);
    ring_buf.allocation_end = ring_buf.allocation ? ring_buf.allocation + alloc_sz : NULL;

    /* Set the buffer size */
    ring_buf.size = alloc_sz;

    /* Use the default allocator */
    ring_buf.allocator = aws_default_allocator();

    /* Initialise head and tail to a nondeterministic but well‑formed state */
    ring_buf.head = (struct aws_atomic_var){ .value = nondet_uint64_t() };
    ring_buf.tail = (struct aws_atomic_var){ .value = nondet_uint64_t() };

    /* Ensure the ring buffer is in a valid state before cleanup */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Keep a copy of the original state for potential later checks */
    struct aws_ring_buffer old = ring_buf;

    /* Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* Post‑conditions: resources released and fields cleared */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.size == 0);
    assert(ring_buf.head.value == 0);
    assert(ring_buf.tail.value == 0);
    /* allocator may remain unchanged after clean_up */
}
