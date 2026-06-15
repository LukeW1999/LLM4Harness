#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>
#include <stddef.h>
#include <stdlib.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

/* Stub for aws_mem_acquire: non‑deterministically return a valid buffer or NULL */
void *aws_mem_acquire(struct aws_allocator *allocator, size_t size) {
    if (nondet_bool()) {
        return NULL;
    }
    uint8_t *mem = (uint8_t *)__CPROVER_malloc(size);
    __CPROVER_assume(mem != NULL);
    return mem;
}

void aws_ring_buffer_init_harness() {
    /* Non‑deterministic input */
    struct aws_ring_buffer *ring_buf = malloc(sizeof(struct aws_ring_buffer));
    struct aws_allocator *allocator = malloc(sizeof(struct aws_allocator));
    size_t size;

    /* Ensure pointers are valid */
    __CPROVER_assume(ring_buf != NULL);
    __CPROVER_assume(allocator != NULL);

    /* Bound the size to avoid pointer‑arithmetic overflow */
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    /* Call the function under test */
    int result = aws_ring_buffer_init(ring_buf, allocator, size);

    /* Postcondition branching on success vs. failure */
    if (result == AWS_OP_SUCCESS) {
        /* 1. Fields that changed (documented via Doxygen) */
        assert(ring_buf->allocation != NULL);
        assert(ring_buf->allocator == allocator);
        assert(ring_buf->allocation_end == ring_buf->allocation + size);
        uint8_t *head = aws_atomic_load_ptr(&ring_buf->head);
        uint8_t *tail = aws_atomic_load_ptr(&ring_buf->tail);
        assert(head == ring_buf->allocation);
        assert(tail == ring_buf->allocation);
    } else {
        /* Failure: the struct was zeroed before the allocation attempt */
        assert(ring_buf->allocation == NULL);
        assert(ring_buf->allocator == NULL);
        assert(ring_buf->allocation_end == NULL);
        /* head and tail are zero‑initialized */
        uint8_t *head = aws_atomic_load_ptr(&ring_buf->head);
        uint8_t *tail = aws_atomic_load_ptr(&ring_buf->tail);
        assert(head == NULL);
        assert(tail == NULL);
    }
}
