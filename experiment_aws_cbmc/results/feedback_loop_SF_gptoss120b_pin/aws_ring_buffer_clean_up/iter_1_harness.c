#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include "make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;

    /* Nondeterministic initialization of the ring buffer */
    ensure_ring_buffer_is_valid(&ring_buf);

    /* Precondition: the ring buffer must satisfy its validity predicate */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Snapshot of relevant state before the call */
    struct aws_allocator *orig_allocator = ring_buf.allocator;
    uint8_t *orig_allocation = ring_buf.allocation;
    uint8_t *orig_allocation_end = ring_buf.allocation_end;
    struct aws_atomic_var orig_head = ring_buf.head;
    struct aws_atomic_var orig_tail = ring_buf.tail;

    /* Call the function under verification */
    aws_ring_buffer_clean_up(&ring_buf);

    /* Postcondition assertions */
    assert(ring_buf.allocator == orig_allocator);               /* allocator unchanged */
    assert(ring_buf.allocation == NULL);                        /* allocation cleared */
    assert(ring_buf.allocation_end == NULL);                    /* allocation_end cleared */
    assert(aws_atomic_load_int(&ring_buf.head) == 0);           /* head reset */
    assert(aws_atomic_load_int(&ring_buf.tail) == 0);           /* tail reset */
    assert(ring_buf.capacity == 0);                             /* capacity reset */
    assert(ring_buf.mask == 0);                                 /* mask reset */
}
