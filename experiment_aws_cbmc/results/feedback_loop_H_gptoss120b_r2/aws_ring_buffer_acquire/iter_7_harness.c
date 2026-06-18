#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_acquire_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0);
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);

    struct aws_ring_buffer ring_buf;
    int init_res = aws_ring_buffer_init(&ring_buf, alloc, alloc_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);

    /* nondeterministic head and tail offsets within the allocation */
    size_t head_offset = nondet_size_t();
    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(head_offset <= alloc_size);
    __CPROVER_assume(tail_offset <= alloc_size);

    uint8_t *head_ptr = ring_buf.allocation + head_offset;
    uint8_t *tail_ptr = ring_buf.allocation + tail_offset;

    aws_atomic_store_ptr(&ring_buf.head, (void *)head_ptr);
    aws_atomic_store_ptr(&ring_buf.tail, (void *)tail_ptr);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_byte_buf dest = {0};

    struct aws_ring_buffer old_ring = ring_buf;
    struct aws_byte_buf old_dest = dest;

    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size <= alloc_size);

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    if (result == AWS_OP_SUCCESS) {
        /* post‑conditions for successful acquire */
        assert(dest.buffer != NULL);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.len <= ring_buf.allocation_end);
        assert(dest.len > 0);
        assert(dest.len <= requested_size);
        assert(dest.capacity >= dest.len);
    } else {
        /* post‑conditions for failed acquire */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(aws_atomic_load_ptr(&ring_buf.head) == aws_atomic_load_ptr(&old_ring.head));
        assert(aws_atomic_load_ptr(&ring_buf.tail) == aws_atomic_load_ptr(&old_ring.tail));
    }

    /* invariants that must hold regardless of success/failure */
    assert(ring_buf.allocator == old_ring.allocator);
    assert(ring_buf.allocation == old_ring.allocation);
    assert(ring_buf.allocation_end == old_ring.allocation_end);
    assert(aws_ring_buffer_is_valid(&ring_buf));

    aws_ring_buffer_clean_up(&ring_buf);
}
