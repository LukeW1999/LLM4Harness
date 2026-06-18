#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_eq_harness(void) {
    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    if (nondet_bool()) {
        size_t len_a = nondet_size_t();
        __CPROVER_assume(len_a <= MAX_STRING_LEN);
        uint8_t a_buf[sizeof(struct aws_string) + MAX_STRING_LEN];
        a = (struct aws_string *)a_buf;
        struct aws_allocator *allocator = aws_default_allocator();
        a->allocator = allocator;
        a->len = len_a;
        for (size_t i = 0; i < len_a; ++i) {
            a->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(a));
    }

    if (nondet_bool()) {
        size_t len_b = nondet_size_t();
        __CPROVER_assume(len_b <= MAX_STRING_LEN);
        uint8_t b_buf[sizeof(struct aws_string) + MAX_STRING_LEN];
        b = (struct aws_string *)b_buf;
        struct aws_allocator *allocator = aws_default_allocator();
        b->allocator = allocator;
        b->len = len_b;
        for (size_t i = 0; i < len_b; ++i) {
            b->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(b));
    }

    struct aws_allocator *old_a_allocator = NULL;
    size_t old_a_len = 0;
    struct aws_allocator *old_b_allocator = NULL;
    size_t old_b_len = 0;
    struct store_byte_from_buffer a_bytes_store = {0};
    struct store_byte_from_buffer b_bytes_store = {0};

    if (a) {
        old_a_allocator = a->allocator;
        old_a_len = a->len;
        save_byte_from_array(a->bytes, a->len, &a_bytes_store);
    }
    if (b) {
        old_b_allocator = b->allocator;
        old_b_len = b->len;
        save_byte_from_array(b->bytes, b->len, &b_bytes_store);
    }

    bool result = aws_string_eq(a, b);

    if (a == b) {
        __CPROVER_assert(result == true, "result true when same pointer");
    } else if (a == NULL || b == NULL) {
        __CPROVER_assert(result == false, "result false when null");
    } else {
        bool expected = aws_array_eq(a->bytes, a->len, b->bytes, b->len);
        __CPROVER_assert(result == expected, "result matches array eq");
    }

    if (a) {
        __CPROVER_assert(a->allocator == old_a_allocator, "allocator unchanged");
        __CPROVER_assert(a->len == old_a_len, "len unchanged");
        assert_byte_from_buffer_matches(a->bytes, &a_bytes_store);
    }
    if (b) {
        __CPROVER_assert(b->allocator == old_b_allocator, "allocator unchanged");
        __CPROVER_assert(b->len == old_b_len, "len unchanged");
        assert_byte_from_buffer_matches(b->bytes, &b_bytes_store);
    }

    if (a) {
        __CPROVER_assert(aws_string_is_valid(a), "a still valid");
    }
    if (b) {
        __CPROVER_assert(aws_string_is_valid(b), "b still valid");
    }
}
