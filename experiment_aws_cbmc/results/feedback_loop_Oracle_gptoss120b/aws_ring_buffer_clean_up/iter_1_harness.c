#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <aws/common/memory.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* Nondeterministic size for allocation */
size_t nondet_size_t(void);

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    size_t size = nondet_size_t();

    /* Limit size to a reasonable range for the proof */
    __CPROVER_assume(size > 0 && size < 1024);

    /* Allocate memory for the ring buffer */
    uint8_t *alloc = malloc(size);
    __CPROVER_assume(alloc != NULL);

    /* Initialize the ring buffer structure */
    memset(&ring_buf, 0, sizeof(ring_buf));
    ring_buf.allocator = aws_default_allocator();
    ring_buf.allocation = alloc;
    ring_buf.allocation_end = alloc + size;
    aws_atomic_store_int(&ring_buf.head, 0);
    aws_atomic_store_int(&ring_buf.tail, 0);

    /* Structural validity assumption */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Call the function under verification */
    aws_ring_buffer_clean_up(&ring_buf);

    /* Post‑condition: the ring buffer must be zeroed */
    struct aws_ring_buffer zeroed = {0};
    assert(memcmp(&ring_buf, &zeroed, sizeof(ring_buf)) == 0);

    return 0;
}
