#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

/* Stub for aws_mem_acquire */
void *aws_mem_acquire(struct aws_allocator *allocator, size_t size) {
    (void)allocator;
    if (nondet_bool()) {
        /* success: allocate memory of size */
        void *ptr = malloc(size);
        if (ptr) {
            __CPROVER_assume(ptr != NULL);
        }
        return ptr;
    } else {
        return NULL;
    }
}

void aws_ring_buffer_init_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(allocator != NULL);

    size_t size;
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* Save old content for unchanged fields (only relevant for failure path) */
    /* Here we will check that on failure, allocator and allocation are NULL. */

    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    if (result == AWS_OP_SUCCESS) {
        /* Success postconditions */
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
        /* head and tail must point to allocation */
        assert(aws_atomic_load_ptr(&ring_buf.head) == ring_buf.allocation);
        assert(aws_atomic_load_ptr(&ring_buf.tail) == ring_buf.allocation);
        assert(AWS_MEM_IS_WRITABLE(ring_buf.allocation, size));
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* Failure postconditions: ring_buf is zeroed by AWS_ZERO_STRUCT */
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocation_end == NULL);
    }

    /* Unchanged fields: the struct is entirely written, so no field remains unchanged */
}
