#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_eq_c_str_harness(void) {
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(array_len < SIZE_MAX);

    uint8_t *array_storage = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(array_storage != NULL);

    uint8_t *c_str_storage = malloc(MAX_BUFFER_SIZE + 1);
    __CPROVER_assume(c_str_storage != NULL);

    uint8_t *old_array_storage = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(old_array_storage != NULL);

    uint8_t *old_c_str_storage = malloc(MAX_BUFFER_SIZE + 1);
    __CPROVER_assume(old_c_str_storage != NULL);

    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        array_storage[i] = nondet_uint8_t();
        c_str_storage[i] = nondet_uint8_t();
    }
    c_str_storage[MAX_BUFFER_SIZE] = nondet_uint8_t();

    size_t terminator_index = nondet_size_t();
    __CPROVER_assume(terminator_index <= MAX_BUFFER_SIZE);
    c_str_storage[terminator_index] = '\0';

    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        old_array_storage[i] = array_storage[i];
        old_c_str_storage[i] = c_str_storage[i];
    }
    old_c_str_storage[MAX_BUFFER_SIZE] = c_str_storage[MAX_BUFFER_SIZE];

    const void *array = array_storage;
    if (array_len == 0 && nondet_bool()) {
        array = NULL;
    }

    const char *c_str = (const char *)c_str_storage;

    __CPROVER_assume(array || array_len == 0);
    if (array_len > 0) {
        __CPROVER_assume(AWS_MEM_IS_READABLE(array, array_len));
    }
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, array_len + 1));

    const void *old_array = array;
    size_t old_array_len = array_len;
    const char *old_c_str = c_str;

    size_t c_str_len = 0;
    bool found_terminator = false;
    for (size_t i = 0; i <= MAX_BUFFER_SIZE; ++i) {
        if (!found_terminator && c_str_storage[i] == '\0') {
            c_str_len = i;
            found_terminator = true;
        }
    }

    bool contents_match = true;
    for (size_t i = 0; i < array_len; ++i) {
        if (((const uint8_t *)array)[i] != c_str_storage[i]) {
            contents_match = false;
        }
    }

    bool expected = found_terminator && array_len == c_str_len && contents_match;

    bool result = aws_array_eq_c_str(array, array_len, c_str);

    if (result) {
        assert(expected);
        assert(found_terminator);
        assert(array_len == c_str_len);
        for (size_t i = 0; i < array_len; ++i) {
            assert(c_str_storage[i] != '\0');
            assert(((const uint8_t *)array)[i] == c_str_storage[i]);
        }
        assert(c_str_storage[array_len] == '\0');
    } else {
        assert(!expected);
    }

    assert(result == expected);

    assert(array == old_array);
    assert(array_len == old_array_len);
    assert(c_str == old_c_str);

    if (array != NULL) {
        for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
            assert(array_storage[i] == old_array_storage[i]);
        }
    }

    for (size_t i = 0; i <= MAX_BUFFER_SIZE; ++i) {
        assert(c_str_storage[i] == old_c_str_storage[i]);
    }

    assert(array || array_len == 0);
    if (array_len > 0) {
        assert(AWS_MEM_IS_READABLE(array, array_len));
    }
    assert(AWS_MEM_IS_READABLE(c_str, array_len + 1));
    assert(found_terminator);
}
