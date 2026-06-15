#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

/* Stub for allocator's mem_acquire */
static void *mem_acquire_stub(struct aws_allocator *allocator, size_t size) {
    void *ptr;
    __CPROVER_assume(ptr == NULL || (ptr != NULL && __CPROVER_is_fresh(ptr, size)));
    return ptr;
}

void aws_ring_buffer_init_harness() {
    /* Input parameters */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator allocator;
    size_t size;

    /* Preconditions */
    allocator.mem_acquire = &mem_acquire_stub;
    __CPROVER_assume(size > 0);

    /* Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, &allocator, size);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocator == &allocator);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation_end == NULL);
        assert(!aws_ring_buffer_is_valid(&ring_buf));
    }
}
