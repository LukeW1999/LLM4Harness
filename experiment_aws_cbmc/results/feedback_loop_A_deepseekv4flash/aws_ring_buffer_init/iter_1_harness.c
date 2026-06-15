#include <aws/common/ring_buffer.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>

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
    struct aws_ring_buffer ring_buf;
    struct aws_allocator allocator;
    size_t size;

    /* Bound the size to avoid pointer‑arithmetic overflow */
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    /* Ensure the input pointers are valid and writable */
    __CPROVER_assume(allocator != NULL && __CPROVER_r_ok(&allocator, sizeof(allocator)));
    __CPROVER_assume(__CPROVER_r_ok(&ring_buf, sizeof(ring_buf)));

    /* Call the function under test */
    int result = aws_ring_buffer_init(&ring_buf, &allocator, size);

    /* Postcondition branching on success vs. failure */
    if (result == AWS_OP_SUCCESS) {
        /* 1. Fields that changed (documented via Doxygen) */
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocator == &allocator);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);
        uint8_t *head = aws_atomic_load_ptr(&ring_buf.head);
        uint8_t *tail = aws_atomic_load_ptr(&ring_buf.tail);
        assert(head == ring_buf.allocation);
        assert(tail == ring_buf.allocation);

        /* 2. Validity invariant */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* Failure: the struct was zeroed before the allocation attempt */
        assert(ring_buf.allocation == NULL);
        assert(ring_buf.allocator == NULL);
        assert(ring_buf.allocation_end == NULL);
        /* head and tail are zero‑initialized; validity check must fail */
        assert(!aws_ring_buffer_is_valid(&ring_buf));
    }

    /* 3. Unchanged fields (implicit: no other fields exist, so nothing more to assert) */
}
