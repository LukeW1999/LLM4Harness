#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static struct aws_string *nondet_valid_aws_string(size_t max_len) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= max_len);

    size_t allocation_size = offsetof(struct aws_string, bytes) + len + 1;
    struct aws_string *str = malloc(allocation_size);
    __CPROVER_assume(str != NULL);

    struct aws_allocator **allocator_field = (struct aws_allocator **)&str->allocator;
    size_t *len_field = (size_t *)&str->len;
    uint8_t *bytes = (uint8_t *)str->bytes;

    if (nondet_bool()) {
        *allocator_field = aws_default_allocator();
    } else {
        *allocator_field = NULL;
    }

    *len_field = len;

    for (size_t i = 0; i < len; ++i) {
        bytes[i] = nondet_uint8_t();
    }
    bytes[len] = 0;

    __CPROVER_assume(aws_string_is_valid(str));
    return str;
}

static bool harness_bytes_equal(const uint8_t *a, size_t a_len, const uint8_t *b, size_t b_len) {
    if (a_len != b_len) {
        return false;
    }

    for (size_t i = 0; i < a_len; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }

    return true;
}

void aws_string_eq_harness(void) {
    struct aws_string *a_alloc = nondet_valid_aws_string(MAX_BUFFER_SIZE);
    struct aws_string *b_alloc = nondet_valid_aws_string(MAX_BUFFER_SIZE);

    const struct aws_string *a;
    const struct aws_string *b;

    if (nondet_bool()) {
        a = NULL;
    } else {
        a = a_alloc;
    }

    if (nondet_bool()) {
        b = a;
    } else if (nondet_bool()) {
        b = NULL;
    } else {
        b = b_alloc;
    }

    struct aws_allocator *a_old_allocator = NULL;
    size_t a_old_len = 0;
    const uint8_t *a_old_bytes = NULL;
    struct store_byte_from_buffer a_byte_storage;

    if (a != NULL) {
        a_old_allocator = a->allocator;
        a_old_len = a->len;
        a_old_bytes = a->bytes;
        save_byte_from_array(a->bytes, a->len + 1, &a_byte_storage);
    }

    struct aws_allocator *b_old_allocator = NULL;
    size_t b_old_len = 0;
    const uint8_t *b_old_bytes = NULL;
    struct store_byte_from_buffer b_byte_storage;

    if (b != NULL) {
        b_old_allocator = b->allocator;
        b_old_len = b->len;
        b_old_bytes = b->bytes;
        save_byte_from_array(b->bytes, b->len + 1, &b_byte_storage);
    }

    bool expected_result;
    if (a == b) {
        expected_result = true;
    } else if (a == NULL || b == NULL) {
        expected_result = false;
    } else {
        expected_result = harness_bytes_equal(a->bytes, a->len, b->bytes, b->len);
    }

    bool result = aws_string_eq(a, b);

    assert(result == expected_result);

    if (result) {
        assert(a == b || (a != NULL && b != NULL && a->len == b->len));
        if (a != NULL && b != NULL && a != b) {
            assert_bytes_match(a->bytes, b->bytes, a->len);
        }
    } else {
        assert(a != b);
        assert(a == NULL || b == NULL || a->len != b->len || !harness_bytes_equal(a->bytes, a->len, b->bytes, b->len));
    }

    if (a != NULL) {
        assert(a->allocator == a_old_allocator);
        assert(a->len == a_old_len);
        assert(a->bytes == a_old_bytes);
        assert_byte_from_buffer_matches(a->bytes, &a_byte_storage);
        assert(aws_string_is_valid(a));
    }

    if (b != NULL) {
        assert(b->allocator == b_old_allocator);
        assert(b->len == b_old_len);
        assert(b->bytes == b_old_bytes);
        assert_byte_from_buffer_matches(b->bytes, &b_byte_storage);
        assert(aws_string_is_valid(b));
    }
}
