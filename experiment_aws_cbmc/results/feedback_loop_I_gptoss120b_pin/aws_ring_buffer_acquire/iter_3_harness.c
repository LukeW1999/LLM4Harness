#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf dest = {0};

    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;

    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0);
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);

    ring_buf.allocation = (uint8_t *)allocator->mem_acquire(allocator, alloc_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + alloc_size;

    size_t head_offset = nondet_size_t();
    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(head_offset <= alloc_size);
    __CPROVER_assume(tail_offset <= alloc_size);
    AWS_ATOMIC_STORE_HEAD_PTR(&ring_buf, ring_buf.allocation + head_offset);
    AWS_ATOMIC_STORE_TAIL_PTR(&ring_buf, ring_buf.allocation + tail_offset);

    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    struct aws_ring_buffer old_ring = ring_buf;
    struct aws_byte_buf old_dest = dest;

    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(dest.buffer >= ring_buf.allocation, "dest.buffer inside allocation");
        __CPROVER_assert(dest.buffer + requested_size <= ring_buf.allocation_end, "dest.buffer within bounds");
        __CPROVER_assert(dest.capacity == requested_size, "dest.capacity correct");
        __CPROVER_assert(dest.len == requested_size, "dest.len correct");
    } else {
        __CPROVER_assert(ring_buf.allocation == old_ring.allocation, "allocation unchanged");
        __CPROVER_assert(ring_buf.allocation_end == old_ring.allocation_end, "allocation_end unchanged");
        __CPROVER_assert(ring_buf.allocator == old_ring.allocator, "allocator unchanged");

        uint8_t *old_head, *old_tail, *new_head, *new_tail;
        AWS_ATOMIC_LOAD_HEAD_PTR(&old_ring, old_head);
        AWS_ATOMIC_LOAD_TAIL_PTR(&old_ring, old_tail);
        AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head);
        AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, new_tail);
        __CPROVER_assert(new_head == old_head, "head unchanged");
        __CPROVER_assert(new_tail == old_tail, "tail unchanged");

        __CPROVER_assert(dest.buffer == old_dest.buffer, "dest.buffer unchanged");
        __CPROVER_assert(dest.capacity == old_dest.capacity, "dest.capacity unchanged");
        __CPROVER_assert(dest.len == old_dest.len, "dest.len unchanged");
    }

    __CPROVER_assert(ring_buf.allocator == old_ring.allocator, "allocator never changes");
    __CPROVER_assert(ring_buf.allocation == old_ring.allocation, "allocation never changes");
    __CPROVER_assert(ring_buf.allocation_end == old_ring.allocation_end, "allocation_end never changes");

    __CPROVER_assert(aws_ring_buffer_is_valid(&ring_buf), "ring buffer remains valid");
}
