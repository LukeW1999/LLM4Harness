#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct aws_string_bounded {
    struct aws_allocator *allocator;
    size_t len;
    uint8_t bytes[MAX_BUFFER_SIZE + 1];
};

static size_t harness_bounded_c_strlen(const char *str) {
    size_t len = 0;
    for (size_t i = 0; i <= MAX_BUFFER_SIZE; ++i) {
        if (str[i] == '\0') {
            return i;
        }
        len++;
    }
    return len;
}

static bool harness_bytes_equal(const uint8_t *a, const uint8_t *b, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

void aws_string_eq_c_str_harness(void) {
    struct aws_string_bounded string_storage;

    string_storage.allocator = nondet_bool() ? aws_default_allocator() : NULL;
    string_storage.len = nondet_size_t();
    __CPROVER_assume(string_storage.len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(string_storage.len < SIZE_MAX);
    string_storage.bytes[string_storage.len] = 0;

    struct aws_string *nonnull_str = (struct aws_string *)&string_storage;
    __CPROVER_assume(aws_string_is_valid(nonnull_str));

    const struct aws_string *str = nondet_bool() ? nonnull_str : NULL;

    char c_str_storage[MAX_BUFFER_SIZE + 1];
    size_t c_str_terminator_index = nondet_size_t();
    __CPROVER_assume(c_str_terminator_index <= MAX_BUFFER_SIZE);
    c_str_storage[c_str_terminator_index] = '\0';

    const char *nonnull_c_str = c_str_storage;
    __CPROVER_assume(aws_c_string_is_valid(nonnull_c_str));

    const char *c_str = nondet_bool() ? nonnull_c_str : NULL;

    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    struct store_byte_from_buffer old_string_byte;
    if (str != NULL) {
        old_allocator = str->allocator;
        old_len = str->len;
        save_byte_from_array(str->bytes, str->len + 1, &old_string_byte);
    }

    const char *old_c_str = c_str;
    struct store_byte_from_buffer old_c_str_byte;
    if (c_str != NULL) {
        save_byte_from_array((const uint8_t *)c_str, MAX_BUFFER_SIZE + 1, &old_c_str_byte);
    }

    bool result = aws_string_eq_c_str(str, c_str);

    bool expected;
    if (str == NULL && c_str == NULL) {
        expected = true;
    } else if (str == NULL || c_str == NULL) {
        expected = false;
    } else {
        size_t c_str_len = harness_bounded_c_strlen(c_str);
        assert(c_str_len <= MAX_BUFFER_SIZE);
        expected = (str->len == c_str_len) && harness_bytes_equal(str->bytes, (const uint8_t *)c_str, str->len);
    }

    assert(result == expected);

    if (result) {
        assert(expected);
        if (str != NULL && c_str != NULL) {
            size_t c_str_len = harness_bounded_c_strlen(c_str);
            assert(str->len == c_str_len);
            assert(harness_bytes_equal(str->bytes, (const uint8_t *)c_str, str->len));
        }
        if (str == NULL) {
            assert(c_str == NULL);
        }
        if (c_str == NULL) {
            assert(str == NULL);
        }
    } else {
        assert(!expected);
        if (str == NULL) {
            assert(c_str != NULL);
        } else if (c_str == NULL) {
            assert(str != NULL);
        } else {
            size_t c_str_len = harness_bounded_c_strlen(c_str);
            assert(str->len != c_str_len || !harness_bytes_equal(str->bytes, (const uint8_t *)c_str, str->len));
        }
    }

    assert(c_str == old_c_str);

    if (str != NULL) {
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);
        assert_byte_from_buffer_matches(str->bytes, &old_string_byte);
        assert(aws_string_is_valid(str));
    }

    if (c_str != NULL) {
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_c_str_byte);
        assert(aws_c_string_is_valid(c_str));
    }
}
