#include <assert.h>
#include <stdlib.h>
#include "aws/common/byte_buf.h"
#include "proof_helpers/make_common_data_structures.h"

/* nondet helpers are declared in the proof helpers header */
bool nondet_bool(void);
size_t nondet_size_t(void);

/* Simple allocator that uses the real malloc/free – sufficient for CBMC */
static void *test_mem_acquire(struct aws_allocator *allocator, size_t size) {
    (void)allocator;
    return malloc(size);
}
static void test_mem_release(struct aws_allocator *allocator, void *ptr) {
    (void)allocator;
    free(ptr);
}
static void *test_mem_realloc(struct aws_allocator *allocator, void *ptr, size_t oldsize, size_t newsize) {
    (void)allocator;
    (void)oldsize;
    return realloc(ptr, newsize);
}
static int test_mem_compare(struct aws_allocator *allocator, const void *a, const void *b, size_t size) {
    (void)allocator;
    return memcmp(a, b, size);
}
static void *test_mem_calloc(struct aws_allocator *allocator, size_t num, size_t size) {
    (void)allocator;
    return calloc(num, size);
}

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf src;
    struct aws_byte_buf dest;
    struct aws_allocator allocator_storage;
    struct aws_allocator *allocator = &allocator_storage;

    /* bound buffers */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    /* ensure buffers have allocated members (may be NULL) */
    ensure_byte_buf_has_allocated_buffer_member(&src);
    ensure_byte_buf_has_allocated_buffer_member(&dest);

    /* src must be a valid buffer */
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* nondeterministically decide whether src.buffer is NULL */
    if (nondet_bool()) {
        src.buffer = NULL;
        src.len = 0;
        src.capacity = 0;
    } else {
        /* keep the allocated buffer, but keep invariants */
        __CPROVER_assume(src.len <= src.capacity);
    }

    /* initialise dest as an empty buffer (as required by the API) */
    dest.buffer = NULL;
    dest.len = 0;
    dest.capacity = 0;
    dest.allocator = NULL;

    /* set up a valid allocator – all function pointers must be non‑NULL */
    allocator->mem_acquire = test_mem_acquire;
    allocator->mem_release = test_mem_release;
    allocator->mem_realloc = test_mem_realloc;
    allocator->mem_compare = test_mem_compare;
    allocator->mem_calloc  = test_mem_calloc;

    /* 2. Save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src  = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 4. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be a valid buffer */
        assert(aws_byte_buf_is_valid(&dest));

        /* allocator is stored */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* when source has no buffer, dest is zeroed */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest gets its own allocation */
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.buffer);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* contents must be identical for src.len bytes */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* failure path – function returns AWS_OP_ERR */
        assert(result == AWS_OP_ERR);

        /* dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* dest must still be a valid (empty) buffer */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* 5. Fields that must not change regardless of result */
    /* src is unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* 6. Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&src));
}
