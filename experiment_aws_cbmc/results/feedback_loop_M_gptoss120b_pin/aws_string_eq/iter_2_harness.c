#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_eq_harness(void) {
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();

    struct aws_string *a = NULL;
    struct aws_string *b = NULL;
    struct aws_allocator *allocator = aws_default_allocator();

    if (!a_is_null) {
        size_t a_len = nondet_size_t();
        __CPROVER_assume(a_len <= MAX_BUFFER_SIZE);
        uint8_t *a_buf = malloc(a_len);
        __CPROVER_assume(a_buf != NULL);
        a = aws_string_new_from_array(allocator, a_buf, a_len);
        __CPROVER_assume(a != NULL);
        __CPROVER_assume(aws_string_is_valid(a));
    }

    if (!b_is_null) {
        size_t b_len = nondet_size_t();
        __CPROVER_assume(b_len <= MAX_BUFFER_SIZE);
        uint8_t *b_buf = malloc(b_len);
        __CPROVER_assume(b_buf != NULL);
        b = aws_string_new_from_array(allocator, b_buf, b_len);
        __CPROVER_assume(b != NULL);
        __CPROVER_assume(aws_string_is_valid(b));
    }

    struct aws_string *old_a = a;
    struct aws_string *old_b = b;

    struct store_byte_from_buffer a_bytes_snapshot = {0};
    struct store_byte_from_buffer b_bytes_snapshot = {0};

    if (a != NULL) {
        save_byte_from_array(a->bytes, a->len, &a_bytes_snapshot);
    }
    if (b != NULL) {
        save_byte_from_array(b->bytes, b->len, &b_bytes_snapshot);
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

    assert(result == expected);

    if (a != NULL) {
        assert(a->allocator == old_a->allocator);
        assert(a->len == old_a->len);
        assert_byte_from_buffer_matches(a->bytes, &a_bytes_snapshot);
        assert(aws_string_is_valid(a));
    } else {
        assert(a == NULL);
    }

    if (b != NULL) {
        assert(b->allocator == old_b->allocator);
        assert(b->len == old_b->len);
        assert_byte_from_buffer_matches(b->bytes, &b_bytes_snapshot);
        assert(aws_string_is_valid(b));
    } else {
        assert(b == NULL);
    }
}
