#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

/* Maximum buffer size for nondeterministic allocation */
#define MAX_BUFFER_SIZE 1024U

/* Nondeterministic size and 64‑bit value generators */
size_t nondet_size_t(void);
uint64_t nondet_uint64_t(void);

/* Allocate a nondeterministic buffer (may be NULL) */
static uint8_t *nondet_allocation(size_t *out_sz) {
    size_t sz = nondet_size_t();
    __CPROVER_assume(sz <= MAX_BUFFER_SIZE);
    *out_sz = sz;
    return sz ? malloc(sz) : NULL;
}

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    size_t alloc_sz;

    /* Underlying storage (may be NULL) */
    ring_buf.allocation = nondet_allocation(&alloc_sz);
    ring_buf.allocation_end = ring_buf.allocation ? ring_buf.allocation + alloc_sz : NULL;
    ring_buf.size = alloc_sz;

    /* Use the default allocator */
    ring_buf.allocator = aws_default_allocator();

    /* Nondeterministic head/tail values that respect ring‑buffer invariants */
    uint64_t head_val = nondet_uint64_t();
    uint64_t tail_val = nondet_uint64_t();
    __CPROVER_assume(head_val <= tail_val);
    __CPROVER_assume(tail_val <= ring_buf.size);
    ring_buf.head = (struct aws_atomic_var){ .value = head_val };
    ring_buf.tail = (struct aws_atomic_var){ .value = tail_val };

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
