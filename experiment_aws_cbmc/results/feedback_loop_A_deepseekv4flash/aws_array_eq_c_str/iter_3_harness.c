#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_eq_c_str_harness() {
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    uint8_t *array = NULL;
    if (array_len > 0) {
        array = (uint8_t *)malloc(array_len);
        __CPROVER_assume(array != NULL);
        for (size_t i = 0; i < array_len; i++) {
            array[i] = nondet_uint8_t();
        }
    } else {
        array = NULL;
    }

    // c_str must be at least array_len+1 to hold the null terminator
    char *c_str = (char *)malloc(array_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < array_len; i++) {
        c_str[i] = nondet_uint8_t();
    }
    c_str[array_len] = '\0';

    bool result = aws_array_eq_c_str(array, array_len, c_str);

    // Compute expected result manually
    bool expected = (c_str[array_len] == '\0') &&
                    (array_len == 0 || memcmp(array, c_str, array_len) == 0);
    assert(result == expected);

    // Additional postcondition checks reachable on both result values
    if (result) {
        assert(array_len == 0 || array != NULL);
        assert(c_str != NULL);
        for (size_t i = 0; i < array_len; i++) {
            assert(array[i] == (uint8_t)c_str[i]);
        }
        assert(c_str[array_len] == '\0');
    } else {
        // If result is false, there must be a mismatch (either length or byte)
        bool mismatch_found = false;
        if (c_str[array_len] != '\0') {
            mismatch_found = true;
        } else {
            for (size_t i = 0; i < array_len; i++) {
                if (array[i] != (uint8_t)c_str[i]) {
                    mismatch_found = true;
                    break;
                }
            }
        }
        assert(mismatch_found);
    }

    if (array_len > 0) {
        free(array);
    }
    free(c_str);
}
