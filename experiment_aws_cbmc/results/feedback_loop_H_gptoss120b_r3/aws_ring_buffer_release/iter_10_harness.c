#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

/* Upper bound for nondeterministic sizes */
#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Choose a non‑zero ring size that is a power of two and bounded */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0 && ring_size < MAX_BUFFER_SIZE);
    __CPROVER_assume((ring_size & (ring_size - 1)) == 0); /* power of two */

    /* Initialise the ring buffer */
    int init_res = aws_ring_buffer_init(&ring, alloc, ring_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* Choose a capacity to acquire (must fit in the ring) */
    size_t cap = nondet_size_t();
    __CPROVER_assume(cap > 0 && cap <= ring_size);

    /* Prepare an empty, but valid, byte buffer for acquire */
    struct aws_byte_buf buf;
    buf.buffer = NULL;
    buf.capacity = 0;
    buf.len = 0;
    buf.allocator = alloc;               /* allocator may be non‑NULL for a valid empty buf */

    /* Acquire a region from the ring – this yields a valid buffer */
    int acquire_res = aws_ring_buffer_acquire(&ring, &buf, cap);
    __CPROVER_assume(acquire_res == AWS_OP_SUCCESS);

    /* Preserve old state for post‑condition checks */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* Call the function under verification */
    aws_ring_buffer_release(&ring, &buf);

    /* Buffer fields must be cleared */
    __CPROVER_assert(buf.buffer == NULL, "buf.buffer cleared");
    __CPROVER_assert(buf.capacity == 0, "buf.capacity cleared");
    __CPROVER_assert(buf.len == 0, "buf.len cleared");
    __CPROVER_assert(buf.allocator == NULL, "buf.allocator cleared");

    /* Compute the expected tail pointer after release (with wrap‑around) */
    uint8_t *expected_tail = old_buf.buffer + old_buf.capacity;
    if (expected_tail >= ring.allocation_end) {
        expected_tail = ring.allocation + (expected_tail - ring.allocation_end);
    }

    /* Tail pointer should have advanced correctly */
    __CPROVER_assert(
        AWS_ATOMIC_LOAD_TAIL_PTR(&ring) == expected_tail,
        "tail pointer advanced correctly");

    /* Head pointer must remain unchanged */
    __CPROVER_assert(
        AWS_ATOMIC_LOAD_HEAD_PTR(&ring) == AWS_ATOMIC_LOAD_HEAD_PTR(&old_ring),
        "head pointer unchanged");

    /* Ring metadata (except tail) must remain unchanged */
    __CPROVER_assert(ring.allocator == old_ring.allocator, "allocator unchanged");
    __CPROVER_assert(ring.allocation == old_ring.allocation, "allocation unchanged");
    __CPROVER_assert(ring.allocation_end == old_ring.allocation_end, "allocation_end unchanged");

    /* Ring must remain valid */
    __CPROVER_assert(aws_ring_buffer_is_valid(&ring), "ring buffer remains valid");
    __CPROVER_assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE), "buf remains bounded");
}
