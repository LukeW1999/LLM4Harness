#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct aws_string_harness_storage {
    struct aws_allocator *allocator;
    size_t len;
    uint8_t bytes[MAX_BUFFER_SIZE + sizeof(struct aws_string) + 1];
};

void aws_string_eq_c_str_harness() {
    struct aws_string_harness_storage str_storage;
    uint8_t c_str_storage[MAX_BUFFER_SIZE + 1];

    size_t str_len = nondet_size_t();
    size_t c_str_len = nondet_size_t();

    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(str_len < SIZE_MAX);
    __CPROVER_assume(c_str_len < SIZE_MAX);

    str_storage.allocator = NULL;
    str_storage.len = str_len;
    str_storage.bytes[str_len] = 0;

    c_str_storage[c_str_len] = 0;
    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        if (i < c_str_len) {
            __CPROVER_assume(c_str_storage[i] != 0);
        }
    }

    bool str_is_null = nondet_bool();
    bool c_str_is_null = nondet_bool();

    const struct aws_string *str = str_is_null ? NULL : (const struct aws_string *)&str_storage;
    const char *c_str = c_str_is_null ? NULL : (const char *)c_str_storage;

    if (str != NULL) {
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (c_str != NULL) {
        __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));
        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    const struct aws_string *old_str_ptr = str;
    const char *old_c_str_ptr = c_str;

    struct aws_allocator *old_allocator = NULL;
    size_t old_str_len = 0;
    uint8_t old_str_bytes[MAX_BUFFER_SIZE + 1];

    if (str != NULL) {
        old_allocator = str->allocator;
        old_str_len = str->len;
        for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
            if (i <= old_str_len) {
                old_str_bytes[i] = str->bytes[i];
            }
        }
    }

    uint8_t old_c_str_bytes[MAX_BUFFER_SIZE + 1];

    if (c_str != NULL) {
        for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
            if (i <= c_str_len) {
                old_c_str_bytes[i] = ((const uint8_t *)c_str)[i];
            }
        }
    }

    bool expected_result;
    if (str == NULL && c_str == NULL) {
        expected_result = true;
    } else if (str == NULL || c_str == NULL) {
        expected_result = false;
    } else if (old_str_len != c_str_len) {
        expected_result = false;
    } else {
        expected_result = true;
        for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
            if (i < old_str_len && old_str_bytes[i] != old_c_str_bytes[i]) {
                expected_result = false;
            }
        }
    }

    bool result = aws_string_eq_c_str(str, c_str);

    if (result) {
        assert(expected_result);
        assert(str == old_str_ptr);
        assert(c_str == old_c_str_ptr);
        if (str != NULL) {
            assert(str->allocator == old_allocator);
            assert(str->len == old_str_len);
        }
    } else {
        assert(!expected_result);
        assert(str == old_str_ptr);
        assert(c_str == old_c_str_ptr);
        if (str != NULL) {
            assert(str->allocator == old_allocator);
            assert(str->len == old_str_len);
        }
    }

    assert(result == expected_result);

    if (old_str_ptr == NULL && old_c_str_ptr == NULL) {
        assert(result == true);
    }

    if ((old_str_ptr == NULL && old_c_str_ptr != NULL) || (old_str_ptr != NULL && old_c_str_ptr == NULL)) {
        assert(result == false);
    }

    assert(str == old_str_ptr);
    assert(c_str == old_c_str_ptr);

    if (str != NULL) {
        assert(str->allocator == old_allocator);
        assert(str->len == old_str_len);
        assert(str->bytes == old_str_ptr->bytes);
        for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
            if (i <= old_str_len) {
                assert(str->bytes[i] == old_str_bytes[i]);
            }
        }
        assert(aws_string_is_valid(str));
    } else {
        assert(str == NULL);
    }

    if (c_str != NULL) {
        for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
            if (i <= c_str_len) {
                assert(((const uint8_t *)c_str)[i] == old_c_str_bytes[i]);
            }
        }
        assert(((const uint8_t *)c_str)[c_str_len] == 0);
        assert(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));
        assert(aws_c_string_is_valid(c_str));
    } else {
        assert(c_str == NULL);
    }
}
