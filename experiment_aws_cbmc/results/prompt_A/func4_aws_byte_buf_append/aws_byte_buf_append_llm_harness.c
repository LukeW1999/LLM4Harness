#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

struct aws_byte_buf {
    size_t len;
    uint8_t *buffer;
    size_t capacity;
    struct aws_allocator *allocator;
};

struct aws_byte_cursor {
    size_t len;
    const uint8_t *ptr;
};

int aws_byte_buf_append(struct aws_byte_buf *to, const struct aws_byte_cursor *from);

void aws_byte_buf_append_harness() {
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    // Non-deterministic initialization
    to.len = nondet_size_t();
    to.capacity = nondet_size_t();
    to.buffer = malloc(to.capacity);
    to.allocator = (struct aws_allocator *)nondet_pointer();

    from.len = nondet_size_t();
    from.ptr = (const uint8_t *)malloc(from.len);

    // Precondition assumptions
    __CPROVER_assume(to.len <= to.capacity);
    __CPROVER_assume(to.capacity == 0 || to.buffer != NULL);
    __CPROVER_assume(from.len == 0 || from.ptr != NULL);

    // Save old state
    size_t old_to_len = to.len;
    uint8_t *old_to_buffer = (uint8_t *)malloc(to.capacity);
    if (to.buffer && old_to_buffer) {
        __CPROVER_array_copy(to.buffer, old_to_buffer, to.capacity);
    }
    size_t old_to_capacity = to.capacity;
    struct aws_allocator *old_to_allocator = to.allocator;

    size_t old_from_len = from.len;
    const uint8_t *old_from_ptr = from.ptr;

    // Call function under test
    int result = aws_byte_buf_append(&to, &from);

    // Postcondition assertions
    if (result == 0) { // Success case
        assert(to.len == old_to_len + from.len);
        assert(to.capacity == old_to_capacity);
        assert(to.allocator == old_to_allocator);
        assert(from.len == old_from_len);
        assert(from.ptr == old_from_ptr);
        for (size_t i = 0; i < old_to_len; i++) {
            assert(to.buffer[i] == old_to_buffer[i]);
        }
        for (size_t i = 0; i < from.len; i++) {
            assert(to.buffer[old_to_len + i] == from.ptr[i]);
        }
    } else { // Failure case
        assert(to.len == old_to_len);
        assert(to.capacity == old_to_capacity);
        assert(to.allocator == old_to_allocator);
        assert(from.len == old_from_len);
        assert(from.ptr == old_from_ptr);
        for (size_t i = 0; i < to.capacity; i++) {
            assert(to.buffer[i] == old_to_buffer[i]);
        }
    }

    // Free allocated memory
    free(to.buffer);
    free(old_to_buffer);
    free((void *)from.ptr);
}
