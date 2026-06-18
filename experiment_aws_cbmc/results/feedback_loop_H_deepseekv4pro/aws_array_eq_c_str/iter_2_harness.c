#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

void aws_array_eq_c_str_harness() {
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= 4);

    const void *array = NULL;
    const char *c_str = NULL;

    bool array_null = nondet_bool();
    if (array_null) {
        __CPROVER_assume(array_len == 0);
        array = NULL;
    } else {
        array = malloc(array_len);
        if (array_len > 0) {
            __CPROVER_assume(array != NULL);
        }
    }

    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= 4);
    c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    ((char *)c_str)[c_str_len] = '\0';

    // Save original contents for immutability check
    uint8_t *old_array = NULL;
    if (array != NULL && array_len > 0) {
        old_array = malloc(array_len);
        memcpy(old_array, array, array_len);
    }
    char *old_c_str = malloc(c_str_len + 1);
    memcpy(old_c_str, c_str, c_str_len + 1);

    // Call function under test
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    // Postconditions: inputs must not be modified
    if (array != NULL && array_len > 0) {
        assert(memcmp(array, old_array, array_len) == 0);
    }
    assert(memcmp(c_str, old_c_str, c_str_len + 1) == 0);

    // Correctness of return value
    bool expected;
    if (array == NULL) {
        expected = (c_str_len == 0);
    } else {
        if (array_len == c_str_len) {
            if (array_len > 0) {
                expected = (memcmp(array, c_str, array_len) == 0);
            } else {
                expected = true;
            }
        } else {
            expected = false;
        }
    }
    assert(result == expected);

    // Memory cleanup
    if (array != NULL) free((void *)array);
    free((void *)c_str);
    if (old_array != NULL) free(old_array);
    free(old_c_str);
}
