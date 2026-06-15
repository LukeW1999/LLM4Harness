#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <aws/common/byte_buf.h>

#define MAX_BUFFER_SIZE 10

// Nondeterministic helpers
uint8_t nondet_uint8_t() { return __CPROVER_nondet_uint8_t(); }
uint32_t nondet_uint32_t() { return __CPROVER_nondet_uint32_t(); }

// Allocate buffer with nondet content; returns NULL on failure
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
    // Nondeterministic but non-null allocator
    struct aws_allocator *allocator = malloc(sizeof(*allocator));
    __CPROVER_assume(allocator != NULL);

    // Source buffer setup
    struct aws_byte_buf src;
    size_t src_capacity = nondet_uint32_t();
    __CPROVER_assume(src_capacity <= MAX_BUFFER_SIZE);
    src.capacity = src_capacity;
    src.buffer = nondet_buffer(src.capacity);
    src.len = nondet_uint32_t();
    __CPROVER_assume(src.len <= src.capacity);
    if (src.capacity > 0) {
        __CPROVER_assume(src.buffer != NULL);
    }

    // Save original state
    struct aws_byte_buf old_src = src;

    // Destination buffer
    struct aws_byte_buf dest;

    // Call the function under verification
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    // Postcondition 1: src unchanged
    assert(src.allocator == old_src.allocator);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    if (src.buffer != NULL && old_src.buffer != NULL && src.len > 0) {
        for (size_t i = 0; i < src.len; i++) {
            assert(src.buffer[i] == old_src.buffer[i]);
        }
    }

    // Postcondition 2: success / failure handling
    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        if (src.buffer == NULL) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer != NULL);
            for (size_t i = 0; i < src.len; i++) {
                assert(dest.buffer[i] == src.buffer[i]);
            }
        }
    } else {
        // Failure: dest should be zeroed
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        assert(aws_byte_buf_is_valid(&dest));
    }
}
