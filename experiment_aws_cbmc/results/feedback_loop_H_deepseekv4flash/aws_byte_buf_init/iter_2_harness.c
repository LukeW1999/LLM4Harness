#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <aws/common/byte_buf.h>

/* Stub for aws_mem_acquire */
void *aws_mem_acquire(struct aws_allocator *allocator, size_t size) {
    __CPROVER_assert(allocator != NULL, "aws_mem_acquire: allocator must not be NULL");
    void *ptr;
    if (nondet_bool()) {
        ptr = malloc(size);
        __CPROVER_assume(ptr != NULL);
    } else {
        ptr = NULL;
    }
    return ptr;
}

void aws_byte_buf_init_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf *buf = malloc(sizeof(*buf));
    __CPROVER_assume(buf != NULL);

    struct aws_allocator *allocator = malloc(sizeof(*allocator));
    __CPROVER_assume(allocator != NULL);

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 2. Save old state BEFORE calling */
    struct aws_byte_buf old = *buf;

    /* 3. Call function under test */
    int result = aws_byte_buf_init(buf, allocator, capacity);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success path: fields are set as expected */
        assert(buf->len == 0);
        assert(buf->capacity == capacity);
        assert(buf->allocator == allocator);
        if (capacity == 0) {
            assert(buf->buffer == NULL);
        } else {
            /* buffer must be allocated, validity already ensures non-NULL */
            assert(buf->buffer != NULL);
        }
    } else {
        /* Failure path: struct is zeroed */
        assert(result == AWS_OP_ERR);
        assert(buf->len == 0);
        assert(buf->buffer == NULL);
        assert(buf->capacity == 0);
        assert(buf->allocator == NULL);
    }

    /* 5. Validity invariants hold after the call */
    /* aws_byte_buf_is_valid(buf) translates to:
       buf != NULL, len <= capacity, and (capacity == 0 || buffer != NULL) */
    assert(buf->len <= buf->capacity);
    assert(buf->capacity == 0 || buf->buffer != NULL);
}
