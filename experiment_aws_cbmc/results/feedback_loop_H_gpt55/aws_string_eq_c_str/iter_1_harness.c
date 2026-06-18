#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct aws_string_harness_storage {
    struct aws_allocator *allocator;
    size_t len;
    uint8_t bytes[MAX_BUFFER_SIZE + 1];
};

void aws_string_eq_c_str_harness() {
    struct aws_string_harness_storage string_storage;
    char c_string_storage[MAX_BUFFER_SIZE + 1];

    size_t str_len = nondet_size_t();
    size_t c_str_len = nondet_size_t();

    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    string_storage.allocator = nondet_bool() ? NULL : aws_default_allocator();
    string_storage.len = str_len;
    string_storage.bytes[str_len] = 0;

    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        if (i < c_str_len) {
            __CPROVER_assume(c_string_storage[i] != '\0');
        }
    }
    c_string_storage[c_str_len] = '\0';

    struct aws_string *str = nondet_bool() ? NULL : (struct aws_string *)&string_storage;
    const char *c_str = nondet_bool() ? NULL : c_string_storage;

    if (str != NULL) {
        __CPROVER_assume(aws_string_is_valid(str));
    }
    if (c_str != NULL) {
        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    struct aws_allocator *old_allocator = string_storage.allocator;
    size_t old_len = string_storage.len;
    const uint8_t *old_string_bytes_ptr = str != NULL ? str->bytes : NULL;
    const char *old_c_str_ptr = c_str;

    uint8_t old_string_bytes[MAX_BUFFER_SIZE + 1];
    uint8_t old_c_string_bytes[MAX_BUFFER_SIZE + 1];

    for (size_t i = 0; i <= MAX_BUFFER_SIZE; ++i) {
        old_string_bytes[i] = string_storage.bytes[i];
        old_c_string_bytes[i] = (uint8_t)c_string_storage[i];
    }

    bool bytes_equal = true;
    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        if (i < old_len) {
            bytes_equal = bytes_equal && (old_string_bytes[i] == (uint8_t)c_string_storage[i]);
        }
    }

    bool expected_result = false;
    if (str == NULL && c_str == NULL) {
        expected_result = true;
    } else if (str != NULL && c_str != NULL) {
        expected_result = (old_len == c_str_len) && bytes_equal;
    }

    bool result = aws_string_eq_c_str(str, c_str);

    assert(result == expected_result);

    if (result) {
        assert(expected_result);
        if (str == NULL || c_str == NULL) {
            assert(str == NULL && c_str == NULL);
        } else {
            assert(old_len == c_str_len);
            assert(bytes_equal);
            assert_bytes_match(str->bytes, (const uint8_t *)c_str, old_len);
            assert(str->allocator == old_allocator);
            assert(str->len == old_len);
            assert(str->bytes == old_string_bytes_ptr);
            assert_bytes_match(str->bytes, old_string_bytes, old_len + 1);
        }
    } else {
        assert(!expected_result);
        if (str == NULL || c_str == NULL) {
            assert(!(str == NULL && c_str == NULL));
        } else {
            assert(old_len != c_str_len || !bytes_equal);
            assert(str->allocator == old_allocator);
            assert(str->len == old_len);
            assert(str->bytes == old_string_bytes_ptr);
            assert_bytes_match(str->bytes, old_string_bytes, old_len + 1);
        }
    }

    if (str != NULL) {
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);
        assert(str->bytes == old_string_bytes_ptr);
        assert_bytes_match(str->bytes, old_string_bytes, old_len + 1);
        assert(aws_string_is_valid(str));
    }

    assert(c_str == old_c_str_ptr);
    if (c_str != NULL) {
        assert_bytes_match((const uint8_t *)c_str, old_c_string_bytes, c_str_len + 1);
        assert(aws_c_string_is_valid(c_str));
    }
}
