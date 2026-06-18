#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256

void aws_array_eq_c_str_harness(void) {
    /* nondeterministic inputs */
    size_t array_len;
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(array_len == 0 || array_len > 0); /* keep CBMC happy */

    /* allocate array (may be NULL when length is 0) */
    const uint8_t *array = NULL;
    if (array_len > 0) {
        uint8_t *tmp = malloc(array_len);
        __CPROVER_assume(tmp != NULL);
        for (size_t i = 0; i < array_len; ++i) {
            tmp[i] = (uint8_t) __CPROVER_nondet_uint();
        }
        array = tmp;
    }

    /* allocate c_str with at least array_len+1 bytes to avoid OOB */
    char *c_str = malloc(array_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i <= array_len; ++i) {
        c_str[i] = (char) __CPROVER_nondet_int();
    }

    /* frame condition: make copies of inputs */
    uint8_t *array_copy = NULL;
    if (array_len > 0) {
        array_copy = malloc(array_len);
        __CPROVER_assume(array_copy != NULL);
        for (size_t i = 0; i < array_len; ++i) {
            array_copy[i] = ((const uint8_t *)array)[i];
        }
    }
    char *c_str_copy = malloc(array_len + 1);
    __CPROVER_assume(c_str_copy != NULL);
    for (size_t i = 0; i <= array_len; ++i) {
        c_str_copy[i] = c_str[i];
    }

    /* call the function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* compute the expected result */
    bool expected = true;
    for (size_t i = 0; i < array_len; ++i) {
        uint8_t s = (uint8_t) c_str[i];
        if (s == '\0' || ((const uint8_t *)array)[i] != s) {
            expected = false;
            break;
        }
    }
    if (expected) {
        expected = ((uint8_t) c_str[array_len]) == '\0';
    }

    /* postcondition: return value matches specification */
    assert(result == expected);

    /* frame condition: inputs unchanged */
    if (array_len > 0) {
        for (size_t i = 0; i < array_len; ++i) {
            assert(((const uint8_t *)array)[i] == array_copy[i]);
        }
    }
    for (size_t i = 0; i <= array_len; ++i) {
        assert(c_str[i] == c_str_copy[i]);
    }

    /* clean up */
    if (array_len > 0) {
        free((void *)array);
        free(array_copy);
    }
    free(c_str);
    free(c_str_copy);

    return 0;
}
