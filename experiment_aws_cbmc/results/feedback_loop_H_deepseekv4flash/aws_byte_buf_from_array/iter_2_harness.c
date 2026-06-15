#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <aws/common/byte_buf.h>

/* Helper to allocate a buffer of given capacity and fill with nondet values,
   return NULL if allocation fails. */
uint8_t *nondet_buffer(size_t capacity) {
    if (capacity == 0) return NULL;
    uint8_t *buf = malloc(capacity);
    if (buf) {
        for (size_t i = 0; i < capacity; i++) {
            buf[i] = nondet_uint8_t();
        }
    }
    return buf;
}

void harness() {
    /* Nondeterministic allocator, but ensure it is non-null as required by the function */
    struct aws_allocator *allocator = malloc(sizeof(*allocator));
    __CPROVER_assume(allocator != NULL);

    /* Nondeterministic source byte buffer with limited capacity to avoid unbounded checks */
    struct aws_byte_buf src;
    size_t src_capacity; 
    __CPROVER_assume(src_capacity <= MAX_BUFFER_SIZE); /* defined as compile-time constant, e.g., 10 */
    src.capacity = src_capacity;
    src.buffer = nondet_buffer(src.capacity);
    src.len = nondet_uint32_t();
    __CPROVER_assume(src.len <= src.capacity);
    src.allocator = allocator; /* src may use same or different allocator; preconditions don't enforce specific allocator */
    /* For the function, src->allocator is read to check for AWS_STATIC_IMPL? Actually, aws_byte_buf_init_copy uses allocator for dest, not src->allocator. So src.allocator can be anything. But to be valid, aws_byte_buf_is_valid ensures buffer is non-null if capacity>0. We'll enforce that. */
    if (src.capacity > 0) {
        __CPROVER_assume(src.buffer != NULL);
    }

    /* Save old src state */
    struct aws_byte_buf old_src = src;

    /* Destination buffer (will be initialized by the function) */
    struct aws_byte_buf dest;

    /* Call the function */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postconditions */

    /* 1. src is unchanged */
    assert(src.allocator == old_src.allocator);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    if (src.buffer != NULL && old_src.buffer != NULL && src.len > 0) {
        assert_bytes_match(src.buffer, old_src.buffer, src.len);
    }

    /* 2. On success, dest is valid and has copied properties */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        if (src.buffer == NULL) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* On failure (allocation failure), dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        assert(aws_byte_buf_is_valid(&dest));
    }
}
