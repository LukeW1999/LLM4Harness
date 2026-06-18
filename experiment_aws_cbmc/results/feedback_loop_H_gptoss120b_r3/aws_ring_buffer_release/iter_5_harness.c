#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Choose a non‑zero ring size bounded by MAX_BUFFER_SIZE */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0 && ring_size < MAX_BUFFER_SIZE);

    /* Initialise the ring buffer */
    int init_res = aws_ring_buffer_init(&ring, alloc, ring_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* Choose an offset within the allocation */
    size_t offset = nondet_size_t();
    __CPROVER_assume(offset < ring_size);

    /* Choose a capacity that fits after the offset (no wrap‑around) */
    size_t cap = nondet_size_t();
    __CPROVER_assume(cap > 0);
    __CPROVER_assume(cap < ring_size - offset); /* strict to avoid wrap */

    /* Construct a byte buffer that represents a previously acquired region */
    struct aws_byte_buf buf;
    buf.buffer = ring.allocation + offset;
    buf.capacity = cap;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = NULL;

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Set the ring's tail to the start of the region we will release */
    AWS_ATOMIC_STORE_TAIL_PTR(&ring, buf.buffer);

    /* Simulate that the region has been acquired by advancing the head */
    AWS_ATOMIC_STORE_HEAD_PTR(&ring, buf.buffer + buf.capacity);

    /* Preserve old state for post‑condition checks */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* Call the function under verification */
    aws_ring_buffer_release(&ring, &buf);

    /* Buffer fields must be cleared */
    __CPROVER_assert(buf.buffer == NULL, "buf.buffer cleared");
    __CPROVER_assert(buf.capacity == 0, "buf.capacity cleared");
    __CPROVER_assert(buf.len == 0, "buf.len cleared");
    __CPROVER_assert(buf.allocator == NULL, "buf.allocator unchanged");

    /* Tail pointer should have advanced by the released capacity */
    __CPROVER_assert(
        AWS_ATOMIC_LOAD_TAIL_PTR(&ring) == old_buf.buffer + old_buf.capacity,
        "tail pointer advanced correctly");

    /* Ring metadata must remain unchanged except tail */
    __CPROVER_assert(ring.allocator == old_ring.allocator, "allocator unchanged");
    __CPROVER_assert(ring.allocation == old_ring.allocation, "allocation unchanged");
    __CPROVER_assert(ring.allocation_end == old_ring.allocation_end, "allocation_end unchanged");
    __CPROVER_assert(
        AWS_ATOMIC_LOAD_HEAD_PTR(&ring) == AWS_ATOMIC_LOAD_HEAD_PTR(&old_ring),
        "head pointer unchanged");

    /* Ring must remain valid */
    __CPROVER_assert(aws_ring_buffer_is_valid(&ring), "ring buffer remains valid");
    __CPROVER_assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE), "buf remains bounded");
}
