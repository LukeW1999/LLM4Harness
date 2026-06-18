#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;
    AWS_ZERO_STRUCT(ring_buf);

    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);
    ring_buf.allocator = alloc;

    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size < 1024);
    ring_buf.allocation = alloc->mem_acquire(alloc, size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = (uint8_t *)ring_buf.allocation + size;
    ring_buf.size = size;

    aws_atomic_store_int(&ring_buf.head, 0);
    aws_atomic_store_int(&ring_buf.tail, 0);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    aws_ring_buffer_clean_up(&ring_buf);

    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.size == 0);
    assert(aws_atomic_load_int(&ring_buf.head) == 0);
    assert(aws_atomic_load_int(&ring_buf.tail) == 0);
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
