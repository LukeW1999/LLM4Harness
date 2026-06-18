#include <proof_helpers/make_common_data_structures.h>

#define MAX_LEN 256

void aws_byte_cursor_eq_byte_buf_harness(void) {
    size_t a_len = nondet_size_t();
    size_t b_len = nondet_size_t();

    __CPROVER_assume(a_len <= MAX_LEN);
    __CPROVER_assume(b_len <= MAX_LEN);

    uint8_t *a_ptr = NULL;
    uint8_t *b_buf = NULL;

    if (a_len > 0) {
        a_ptr = malloc(a_len);
        __CPROVER_assume(a_ptr != NULL);
    }

    if (b_len > 0) {
        b_buf = malloc(b_len);
        __CPROVER_assume(b_buf != NULL);
    }

    struct aws_byte_cursor a = {
        .len = a_len,
        .ptr = a_ptr
    };

    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf b = {
        .len = b_len,
        .capacity = b_len,
        .buffer = b_buf,
        .allocator = allocator
    };

    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    struct aws_byte_cursor a_orig = a;
    struct aws_byte_buf b_orig = b;

    uint8_t *a_ptr_copy = NULL;
    uint8_t *b_buf_copy = NULL;

    if (a_len > 0) {
        a_ptr_copy = malloc(a_len);
        __CPROVER_assume(a_ptr_copy != NULL);
        memcpy(a_ptr_copy, a_ptr, a_len);
    }

    if (b_len > 0) {
        b_buf_copy = malloc(b_len);
        __CPROVER_assume(b_buf_copy != NULL);
        memcpy(b_buf_copy, b_buf, b_len);
    }

    bool result = aws_byte_cursor_eq_byte_buf(&a, &b);
    bool expected = aws_array_eq(a.ptr, a.len, b.buffer, b.len);
    assert(result == expected);

    assert(a.len == a_orig.len);
    assert(a.ptr == a_orig.ptr);
    assert(b.len == b_orig.len);
    assert(b.capacity == b_orig.capacity);
    assert(b.buffer == b_orig.buffer);
    assert(b.allocator == b_orig.allocator);

    if (a_len > 0) {
        assert(memcmp(a.ptr, a_ptr_copy, a_len) == 0);
    }
    if (b_len > 0) {
        assert(memcmp(b.buffer, b_buf_copy, b_len) == 0);
    }
}
