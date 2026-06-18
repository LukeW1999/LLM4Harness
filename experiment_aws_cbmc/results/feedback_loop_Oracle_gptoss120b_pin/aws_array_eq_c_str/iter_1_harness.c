#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/* Maximum size for nondeterministic buffers */
#define MAX_BUFFER_SIZE 256

void aws_array_eq_c_str_harness(void) {
    /* nondeterministic inputs */
    size_t array_len;
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(array_len == 0 || array_len > 0); /* keep it concrete */

    /* Allocate array (may be NULL when length is 0) */
    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
        /* fill with nondet bytes */
        for (size_t i = 0; i < array_len; ++i) {
            array[i] = (uint8_t) __CPROVER_nondet_uint();
        }
    }
    __CPROVER_assume(array || (array_len == 0));

    /* Allocate c_str with at least array_len+1 bytes */
    size_t c_str_buf_len = array_len + 1;
    char *c_str = malloc(c_str_buf_len);
    __CPROVER_assume(c_str != NULL);
    /* fill with nondet bytes */
    for (size_t i = 0; i < c_str_buf_len; ++i) {
        c_str[i] = (char) __CPROVER_nondet_int();
    }
    /* Ensure there is a terminating null at or after array_len to avoid OOB reads */
    c_str[array_len] = '\0';
    /* Optionally place an early null before array_len */
    if (array_len > 0) {
        bool early_null = __CPROVER_nondet_bool();
        if (early_null) {
            size_t idx = __CPROVER_nondet_uint() % array_len;
            c_str[idx] = '\0';
        }
    }
    __CPROVER_assume(c_str);

    /* Save copies for frame condition checks */
    uint8_t *array_copy = NULL;
    if (array_len > 0) {
        array_copy = malloc(array_len);
        __CPROVER_assume(array_copy != NULL);
        for (size_t i = 0; i < array_len; ++i) {
            array_copy[i] = array[i];
        }
    }
    char *c_str_copy = malloc(c_str_buf_len);
    __CPROVER_assume(c_str_copy != NULL);
    for (size_t i = 0; i < c_str_buf_len; ++i) {
        c_str_copy[i] = c_str[i];
    }

    /* Call the function under verification */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* Compute the expected result according to the specification */
    bool expected = true;
    for (size_t i = 0; i < array_len; ++i) {
        uint8_t s = (uint8_t) c_str[i];
        if (s == '\0') {
            expected = false;
            break;
        }
        if (array[i] != s) {
            expected = false;
            break;
        }
    }
    if (expected) {
        expected = (c_str[array_len] == '\0');
    }

    /* Postcondition 1: return value correctness */
    assert(result == expected);

    /* Postcondition 2: frame condition – inputs must be unchanged */
    if (array_len > 0) {
        for (size_t i = 0; i < array_len; ++i) {
            assert(array[i] == array_copy[i]);
        }
    }
    for (size_t i = 0; i < c_str_buf_len; ++i) {
        assert(c_str[i] == c_str_copy[i]);
    }

    /* Clean up */
    free(array);
    free(array_copy);
    free(c_str);
    free(c_str_copy);

    return 0;
}
