#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <aws/common/byte_buf.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

/* Stub for aws_mem_acquire */
void *aws_mem_acquire(struct aws_allocator *allocator, size_t size) {
    __CPROVER_assert(allocator != NULL, "aws_mem_acquire: allocator must not be NULL");
    void *ptr;
    if (nondet_bool()) {
        ptr = malloc(size);
        __CPROVER_assume(ptr != NULL && size > 0);
    } else {
        ptr = NULL;
    }
    return ptr;
}

void aws_byte_buf_init_harness() {
    /* Allocate and assume non-null for buf and allocator */
    struct aws_byte_buf *buf = malloc(sizeof(*buf));
    __CPROVER_assume(buf != NULL);

    struct aws_allocator *allocator = malloc(sizeof(*allocator));
    __CPROVER_assume(allocator != NULL);

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* Save initial state */
    struct aws_byte_buf old = *buf;

    /* Call the function under test */
    int result = aws_byte_buf_init(buf, allocator, capacity);

    /* Postconditions for success path */
    if (result == AWS_OP_SUCCESS) {
        assert(buf->len == 0);
        assert(buf->capacity == capacity);
        assert(buf->allocator == allocator);
        if (capacity == 0) {
            assert(buf->buffer == NULL);
        } else {
            assert(buf->buffer != NULL);
        }
    } else {
        /* Failure path – struct should be zeroed */
        assert(result == AWS_OP_ERR);
        assert(buf->len == 0);
        assert(buf->buffer == NULL);
        assert(buf->capacity == 0);
        assert(buf->allocator == NULL);
    }

    /* General invariants that should hold after the call */
    assert(buf->len <= buf->capacity);
    assert(buf->capacity == 0 || buf->buffer != NULL);
}
