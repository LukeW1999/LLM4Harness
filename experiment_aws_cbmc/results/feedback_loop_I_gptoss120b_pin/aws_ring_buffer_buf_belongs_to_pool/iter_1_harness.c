#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf buf;

    /* Allocate and bound the ring buffer's internal allocation */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);
    ring_buf.allocation = (uint8_t *)malloc(ring_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + ring_size;

    /* Initialize other ring buffer fields */
    ring_buf.allocator = aws_default_allocator();
    /* head and tail are atomic vars; initialize them nondeterministically */
    ring_buf.head = (struct aws_atomic_var){ .value = nondet_size_t() };
    ring_buf.tail = (struct aws_atomic_var){ .value = nondet_size_t() }
