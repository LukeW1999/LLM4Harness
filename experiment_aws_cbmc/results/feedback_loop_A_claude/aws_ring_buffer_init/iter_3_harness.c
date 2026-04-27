#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

/* Stub allocator that can be made to fail */
static int s_alloc_should_fail;

static void *s_stub_mem_acquire(struct aws_allocator *allocator, size_t size) {
    (void)allocator;
    if (s_alloc_should_fail) {
        return NULL;
    }
    return malloc(size);
}

static void s_stub_mem_release(struct aws_allocator *allocator, void *ptr) {
    (void)allocator;
    free(ptr);
}

static void *s_stub_mem_realloc(struct aws_allocator *allocator, void *old_ptr, size_t old_size, size_t new_size) {
    (void)allocator;
    (void)old_size;
    if (s_alloc_should_fail) {
        return NULL;
    }
    return realloc(old_ptr, new_size);
}

static void *s_stub_mem_calloc(struct aws_allocator *allocator, size_t num, size_t size) {
    (void)allocator;
    if (s_alloc_should_fail) {
        return NULL;
    }
    return calloc(num, size);
}

static struct aws_allocator s_stub_allocator = {
    .mem_acquire = s_stub_mem_acquire,
    .mem_release = s_stub_mem_release,
    .mem_realloc = s_stub_mem_realloc,
    .mem_calloc = s_stub_mem_calloc,
};

void aws_ring_buffer_init_harness() {
    /* Nondeterministic choice: should allocation fail? */
    __CPROVER_assume(s_alloc_should_fail == 0 || s_alloc_should_fail == 1);

    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = &s_stub_allocator;

    size_t size;
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* At least one assertion must always fire */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* On success: allocation must be non-null */
        assert(ring_buf.allocation != NULL);

        /* allocator must be set to the provided allocator */
        assert(ring_buf.allocator == allocator);

        /* allocation_end must be allocation + size */
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        /* head and tail must point to allocation (ring buffer is empty after init) */
        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
        assert(head_ptr == ring_buf.allocation);
        assert(tail_ptr == ring_buf.allocation);

        /* allocation_end must be strictly greater than allocation (size > 0) */
        assert(ring_buf.allocation_end > ring_buf.allocation);

        /* Clean up */
        aws_ring_buffer_clean_up(&ring_buf);
    } else {
        /* On failure: result must be AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
    }
}
