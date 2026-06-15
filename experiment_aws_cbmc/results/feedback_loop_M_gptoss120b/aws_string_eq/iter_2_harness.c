#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_harness(void) {
    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    bool a_is_null = nondet_bool();
    __CPROVER_assume(a_is_null == true || a_is_null == false);
    if (!a_is_null) {
        size_t len_a = nondet_size_t();
        __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
        uint8_t buf_a[MAX_BUFFER_SIZE];
        for (size_t i = 0; i < len_a; ++i) {
            buf_a[i] = nondet_uint8_t();
        }
        a = aws_string_new_from_array(aws_default_allocator(), buf_a, len_a);
        __CPROVER_assume(a != NULL);
        __CPROVER_assume(aws_string_is_valid(a));
    }

    bool b_is_null = nondet_bool();
    __CPROVER_assume(b_is_null == true || b_is_null == false);
    if (!b_is_null) {
        size_t len_b = nondet_size_t();
        __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);
        uint8_t buf_b[MAX_BUFFER_SIZE];
        for (size_t i = 0; i < len_b; ++i) {
            buf_b[i] = nondet_uint8_t();
        }
        b = aws_string_new_from_array(aws_default_allocator(), buf_b, len_b);
        __CPROVER_assume(b != NULL);
        __CPROVER_assume(aws_string_is_valid(b));
    }

    struct aws_string old_a;
    struct aws_string old_b;
    struct store_byte_from_buffer old_a_byte;
    struct store_byte_from_buffer old_b_byte;

    if (a != NULL) {
        old_a = *a;
        if (a->len > 0) {
            save_byte_from_array(a->bytes, a->len, &old_a_byte);
        }
    }
    if (b != NULL) {
        old_b = *b;
        if (b->len > 0) {
            save_byte_from_array(b->bytes, b->len, &old_b_byte);
        }
    }

    bool result = aws_string_eq(a, b);

    if (a == b) {
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        assert(result == false);
    } else {
        bool eq = aws_array_eq(a->bytes, a->len, b->bytes, b->len);
        assert(result == eq);
    }

    if (a != NULL) {
        assert(a->allocator == old_a.allocator);
        assert(a->len == old_a.len);
        if (a->len > 0) {
            assert_byte_from_buffer_matches(a->bytes, &old_a_byte);
        }
    }

    if (b != NULL) {
        assert(b->allocator == old_b.allocator);
        assert(b->len == old_b.len);
        if (b->len > 0) {
            assert_byte_from_buffer_matches(b->bytes, &old_b_byte);
        }
    }

    if (a != NULL) {
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(aws_string_is_valid(b));
    }
}
