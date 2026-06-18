#include <aws/common/byte_buf.h>
#include <proof_helpers/nondet.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void aws_array_eq_c_str_harness() {
    // Bounded array length
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    // Allocate array buffer (NULL allowed only if len == 0)
    uint8_t *array = NULL;
    if (array_len > 0) {
        array = (uint8_t *)malloc(array_len);
        __CPROVER_assume(array != NULL);
    } else {
        if (nondet_bool()) {
            array = NULL;
        } else {
            array = (uint8_t *)malloc(1);
        }
    }

    // Fill array bytes nondeterministically
    for (size_t i = 0; i < array_len; i++) {
        array[i] = nondet_uint8_t();
    }

    // Save original array contents
    uint8_t orig_array[MAX_BUFFER_SIZE];
    if (array_len > 0) {
        memcpy(orig_array, array, array_len);
    }

    // Bounded C‑string length and allocation
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = (char *)malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);

    // Fill string bytes nondeterministically, ensure null terminator
    for (size_t i = 0; i < c_str_len; i++) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[c_str_len] = '\0';

    // Save original C‑string contents
    char orig_c_str[MAX_BUFFER_SIZE + 1];
    memcpy(orig_c_str, c_str, c_str_len + 1);

    // Compute expected result according to the specification
    size_t actual_c_str_len = strlen(c_str);
    bool expected;
    if (actual_c_str_len != array_len) {
        expected = false;
    } else {
        if (array_len == 0) {
            expected = true;   // empty array matches empty string
        } else {
            expected = (memcmp(array, c_str, array_len) == 0);
        }
    }

    // Call the function under test
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    // Assert that the returned value matches the specification
    assert(result == expected);

    // Immutability: array and C‑string must not be modified
    if (array_len > 0) {
        assert(memcmp(array, orig_array, array_len) == 0);
    }
    assert(memcmp(c_str, orig_c_str, c_str_len + 1) == 0);

    // Clean up allocated memory
    if (array_len > 0) {
        free(array);
    } else {
        if (array != NULL) free(array);
    }
    free(c_str);
}
