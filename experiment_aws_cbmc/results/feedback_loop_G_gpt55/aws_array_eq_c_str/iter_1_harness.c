#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_eq_c_str_harness() {
    uint8_t array_storage[MAX_BUFFER_SIZE];
    char c_str_storage[MAX_BUFFER_SIZE + 1];

    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(array_len < SIZE_MAX);

    c_str_storage[MAX_BUFFER_SIZE] = '\0';

    bool use_null_array = nondet_bool();
    const void *array = use_null_array ? NULL : (const void *)array_storage;
    const char *c_str = c_str_storage;

    __CPROVER_assume(array || (array_len == 0));
    __CPROVER_assume(c_str != NULL);

    assert(array || (array_len == 0));
    assert(c_str != NULL);
    assert(AWS_MEM_IS_READABLE(c_str, array_len + 1));
    if (array_len > 0) {
        assert(array != NULL);
        assert(AWS_MEM_IS_READABLE(array, array_len));
    }

    uint8_t old_array_storage[MAX_BUFFER_SIZE];
    char old_c_str_storage[MAX_BUFFER_SIZE + 1];

    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        old_array_storage[i] = array_storage[i];
    }
    for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
        old_c_str_storage[i] = c_str_storage[i];
    }

    bool expected = true;
    const uint8_t *array_bytes = (const uint8_t *)array;
    const uint8_t *str_bytes = (const uint8_t *)c_str;

    for (size_t i = 0; i < array_len; ++i) {
        if (str_bytes[i] == '\0') {
            expected = false;
        }
        if (array_bytes[i] != str_bytes[i]) {
            expected = false;
        }
    }
    if (str_bytes[array_len] != '\0') {
        expected = false;
    }

    bool result = aws_array_eq_c_str(array, array_len, c_str);

    if (result) {
        assert(expected);
        assert(str_bytes[array_len] == '\0');
        for (size_t i = 0; i < array_len; ++i) {
            assert(array_bytes[i] == str_bytes[i]);
            assert(array_bytes[i] != '\0');
            assert(str_bytes[i] != '\0');
        }
    } else {
        assert(!expected);
    }

    assert(result == expected);

    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        assert(array_storage[i] == old_array_storage[i]);
    }
    for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
        assert(c_str_storage[i] == old_c_str_storage[i]);
    }

    assert(array || (array_len == 0));
    assert(c_str != NULL);
    assert(AWS_MEM_IS_READABLE(c_str, array_len + 1));
    assert(c_str_storage[MAX_BUFFER_SIZE] == '\0');
    if (array_len > 0) {
        assert(array != NULL);
        assert(AWS_MEM_IS_READABLE(array, array_len));
    }
}
