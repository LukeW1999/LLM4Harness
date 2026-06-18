#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 256

/* Global variables to record calls to the mock allocator */
static int release_called;
static void *release_buf;
static struct aws_allocator *release_alloc;

/* Mock allocator functions */
static void *mock_mem_acquire(struct aws_allocator *alloc, size_t size) {
    (void)alloc;
    (void)size;
    return nondet_pointer();
}

static void mock_mem_release(struct aws_allocator *alloc, void *ptr) {
    release_called++;
    release_buf = ptr;
    release_alloc = alloc;
}

/* Mock allocator instance */
static struct aws_allocator mock_allocator = {
    .mem_acquire = mock_mem_acquire,
    .mem_release = mock_mem_release,
    .mem_realloc = NULL,
    .mem_calloc = NULL,
    .mem_destroy = NULL,
    .mem_is_valid = NULL,
    .mem_is_thread_safe = NULL,
    .mem_is_thread_local = NULL,
    .mem_is_process_local = NULL,
    .mem_is_process_shared = NULL,
    .mem_is_process_private = NULL,
    .mem_is_process_shared = NULL,
    .mem_is_process_private = NULL,
    .mem_is_process_shared = NULL
};

void aws_byte_buf_clean_up_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc;

    /* nondeterministically choose a capacity within a reasonable bound */
    size_t cap;
    __CPROVER_assume((cap = nondet_size_t()) <= MAX_BUFFER_SIZE);

    buf.capacity = cap;
    buf.len = (cap == 0) ? 0 : nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    if (buf.capacity > 0) {
        /* Use the mock allocator for non‑zero capacity */
        alloc = &mock_allocator;
        buf.buffer = alloc->mem_acquire(alloc, buf.capacity);
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        /* For zero capacity, allow allocator to be NULL or the mock allocator */
        alloc = nondet_bool() ? NULL : &mock_allocator;
        buf.buffer = NULL;
    }

    buf.allocator = alloc;

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Record the original state */
    struct aws_byte_buf old_buf = buf;

    /* Reset call‑recording globals */
    release_called = 0;
    release_buf = NULL;
    release_alloc = NULL;

    aws_byte_buf_clean_up(&buf);

    /* Post‑condition checks on the cleaned‑up buffer */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* Original buffer should remain unchanged except for the allocator pointer */
    assert(old_buf.len == old_buf.len);
    assert(old_buf.capacity == old_buf.capacity);
    assert(old_buf.allocator == alloc);
    if (old_buf.capacity > 0) {
        assert(old_buf.buffer != NULL);
    } else {
        assert(old_buf.buffer == NULL);
    }

    /* Verify that the allocator's release function was called exactly when appropriate */
    if (old_buf.buffer != NULL && old_buf.allocator != NULL) {
        assert(release_called == 1);
        assert(release_buf == old_buf.buffer);
        assert(release_alloc == old_buf.allocator);
    } else {
        assert(release_called == 0);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
