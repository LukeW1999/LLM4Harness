#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;

    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);
    __CPROVER_assume(alloc->mem_release != NULL);
    ring_buf.allocator = alloc;

    size_t size = nondet_uint();
    __CPROVER_assume(size > 0 && size < 1024);
    ring_buf.allocation = alloc->mem_acquire(alloc, size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = (uint8_t *)ring_buf.allocation + size;
    ring_buf.size = size;

    ring_buf.head = (struct aws_atomic_var){0};
    ring_buf.tail = (struct aws_atomic_var){0};

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    aws_ring_buffer_clean_up(&ring_buf);

    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.size == 0);
    assert(ring_buf.head == (struct aws_atomic_var){0});
    assert(ring_buf.tail == (struct aws_atomic_var){0});
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
