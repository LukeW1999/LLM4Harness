#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_eq_c_str_harness() {
    size_t array_len = nondet_size_t();
    size_t c_str_len = nondet_size_t();

    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    uint8_t array_storage[MAX_BUFFER_SIZE + 1];
    uint8_t c_str_storage[MAX_BUFFER_SIZE + 1];

    for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
        array_storage[i] = nondet_uint8_t();
        c_str_storage[i] = nondet_uint8_t();
    }

    for (size_t i = 0; i < c_str_len; ++i) {
        __CPROVER_assume(c_str_storage[i] != '\0');
    }
    c_str_storage[c_str_len] = '\0';

    const uint8_t *array = array_storage;
    if (nondet_bool()) {
        array = NULL;
    }

    __CPROVER_assume(array || array_len == 0);

    const char *c_str = (const char *)c_str_storage;

    __CPROVER_assume(AWS_MEM_IS_READABLE(array, array_len));
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));

    const uint8_t *old_array = array;
    const char *old_c_str = c_str;
    size_t old_array_len = array_len;
    size_t old_c_str_len = c_str_len;

    uint8_t old_array_storage[MAX_BUFFER_SIZE + 1];
    uint8_t old_c_str_storage[MAX_BUFFER_SIZE + 1];

    for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
        old_array_storage[i] = array_storage[i];
        old_c_str_storage[i] = c_str_storage[i];
    }

    bool result = aws_array_eq_c_str(array, array_len, c_str);

    bool contents_match = true;
    if (array_len == c_str_len) {
        for (size_t i = 0; i < array_len; ++i) {
            if (array[i] != c_str_storage[i]) {
                contents_match = false;
            }
        }
    }

    bool expected_result = (array_len == c_str_len) && contents_match;

    bool array_contains_null = false;
    for (size_t i = 0; i < array_len; ++i) {
        if (array[i] == '\0') {
            array_contains_null = true;
        }
    }

    if (result) {
        assert(array_len == c_str_len);
        assert(contents_match);
        assert(expected_result);
        assert(c_str_storage[array_len] == '\0');

        for (size_t i = 0; i < array_len; ++i) {
            assert(array[i] == c_str_storage[i]);
            assert(array[i] != '\0');
            assert(c_str_storage[i] != '\0');
        }
    } else {
        assert(!expected_result);
        assert(array_len != c_str_len || !contents_match);
    }

    if (array_contains_null) {
        assert(!result);
    }

    assert(result == expected_result);

    assert(array == old_array);
    assert(c_str == old_c_str);
    assert(array_len == old_array_len);
    assert(c_str_len == old_c_str_len);

    for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
        assert(array_storage[i] == old_array_storage[i]);
        assert(c_str_storage[i] == old_c_str_storage[i]);
    }

    assert(array || array_len == 0);
    assert(AWS_MEM_IS_READABLE(array, array_len));
    assert(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));
    assert(c_str_storage[c_str_len] == '\0');

    for (size_t i = 0; i < c_str_len; ++i) {
        assert(c_str_storage[i] != '\0');
    }
}
