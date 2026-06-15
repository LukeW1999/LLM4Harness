#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256U

void aws_ring_buffer_clean_up_harness(void) {
    /* 1. Declare the ring buffer */
    struct aws_ring_buffer ring_buf;

    /* 2. Allocate a non‑deterministic buffer */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0);
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);
    ring_buf.allocation = (uint8_t *)malloc(alloc_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + alloc_size;

    /* 3. Use the default allocator */
    ring_buf.allocator = aws_default_allocator();

    /* 4. Initialise atomic head/tail within the buffer bounds */
    uint32_t head_val = nondet_uint32_t() % (alloc_size + 1);
    uint32_t tail_val = nondet_uint32_t() % (alloc_size + 1);
    __CPROVER_assume(head_val <= tail_val);
    ring_buf.head = (struct aws_atomic_var){ .value = (int)head_val };
    ring_buf.tail = (struct aws_atomic_var){ .value = (int)tail_val };

    /* 5. Assume the ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 6. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 7. Post‑conditions */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.allocator == NULL);
    assert(aws_atomic_load_int(&ring_buf.head) == 0);
    assert(aws_atomic_load_int(&ring_buf.tail) == 0);

    /* 8. The zeroed ring buffer must still be valid */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
