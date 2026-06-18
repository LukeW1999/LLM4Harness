#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_eq_harness(void) {
    aws_common_library_init();

    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    if (nondet_bool()) {
        a = NULL;
    } else {
        size_t len_a = nondet_size_t();
        __CPROVER_assume(len_a <= MAX_STRING_LEN);
        uint8_t *buf_a = (uint8_t *)malloc(len_a);
        __CPROVER_assume(buf_a != NULL);
        for (size_t i = 0; i < len_a; ++i) {
            buf_a[i] = nondet_uint8_t();
        }
        a = aws_string_new_from_array(allocator, buf_a, len_a);
        free(buf_a);
        __CPROVER_assume(aws_string_is_valid(a));
    }

    if (nondet_bool()) {
        b = NULL;
    } else {
        size_t len_b = nondet_size_t();
        __CPROVER_assume(len_b <= MAX_STRING_LEN);
        uint8_t *buf_b = (uint8_t *)malloc(len_b);
        __CPROVER_assume(buf_b != NULL);
        for (size_t i = 0; i < len_b; ++i) {
            buf_b[i] = nondet_uint8_t();
        }
        b = aws_string_new_from_array(allocator, buf_b, len_b);
        free(buf_b);
        __CPROVER_assume(aws_string_is_valid(b));
    }

    struct aws_allocator *old_a_allocator = NULL;
    size_t old_a_len = 0;
    struct store_byte_from_buffer a_storage = {0};

    struct aws_allocator *old_b_allocator = NULL;
    size_t old_b_len = 0;
    struct store_byte_from_buffer b_storage = {0};

    if (a != NULL) {
        old_a_allocator = a->allocator;
        old_a_len = a->len;
        save_byte_from_array(a->bytes, a->len, &a_storage);
    }
    if (b != NULL) {
        old_b_allocator = b->allocator;
        old_b_len = b->len;
        save_byte_from_array(b->bytes, b->len, &b_storage);
    }

    bool result = aws_string_eq(a, b);

    bool expected;
    if (a == b) {
        expected = true;
    } else if (a == NULL || b == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq(a->bytes, a->len, b->bytes, b->len);
    }
    __CPROVER_assert(result == expected, "aws_string_eq result matches expected");

    if (a != NULL) {
        __CPROVER_assert(a->allocator == old_a_allocator, "a allocator unchanged");
        __CPROVER_assert(a->len == old_a_len, "a len unchanged");
        assert_byte_from_buffer_matches(a->bytes, &a_storage);
    }
    if (b != NULL) {
        __CPROVER_assert(b->allocator == old_b_allocator, "b allocator unchanged");
        __CPROVER_assert(b->len == old_b_len, "b len unchanged");
        assert_byte_from_buffer_matches(b->bytes, &b_storage);
    }

    __CPROVER_assert(aws_string_is_valid(a), "a valid");
    __CPROVER_assert(aws_string_is_valid(b), "b valid");

    if (a) aws_string_destroy(a);
    if (b) aws_string_destroy(b);
}
