#include <assert.h>
#include <stdlib.h>
#include <string.h>
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
    /* allocator */
    struct aws_allocator allocator_storage;
    struct aws_allocator *allocator = &allocator_storage;
    allocator->mem_acquire = test_mem_acquire;
    allocator->mem_release = test_mem_release;
    allocator->mem_realloc = test_mem_realloc;
    allocator->mem_compare = test_mem_compare;
    allocator->mem_calloc  = test_mem_calloc;

    /* source buffer */
    struct aws_byte_buf src;
    src.allocator = allocator;

    size_t cap = nondet_size_t();
    __CPROVER_assume(cap <= MAX_BUFFER_SIZE);

    if (nondet_bool()) {
        /* empty source */
        src.buffer = NULL;
        src.len = 0;
        src.capacity = 0;
    } else {
        src.buffer = test_mem_acquire(allocator, cap);
        src.capacity = cap;
        src.len = nondet_size_t();
        __CPROVER_assume(src.len <= src.capacity);
        if (src.buffer == NULL) {
            /* allocation failed – keep buffer empty */
            src.capacity = 0;
            src.len = 0;
        }
    }

    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* destination buffer – must be empty before the call */
    struct aws_byte_buf dest;
    dest.buffer = NULL;
    dest.len = 0;
    dest.capacity = 0;
    dest.allocator = NULL;

    /* Save old state for later comparison */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src  = src;

    /* Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.allocator == allocator);
        if (src.buffer == NULL) {
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.buffer);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* src must remain unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(aws_byte_buf_is_valid(&src));
}
