#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>

#define MAX_BUFFER_SIZE 256

/* nondeterministic helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);
char nondet_char(void);

void aws_array_eq_c_str_harness(void) {
    /*--- Setup preconditions ---*/
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(array_len == 0 || array_len > 0); /* keep size_t non‑negative */

    /* allocate and initialize array */
    const uint8_t *array = NULL;
    uint8_t *array_buf = NULL;
    if (array_len > 0) {
        array_buf = malloc(array_len);
        __CPROVER_assume(array_buf != NULL);
        for (size_t i = 0; i < array_len; ++i) {
            array_buf[i] = nondet_uint8_t();
        }
        array = array_buf;
    }
    __CPROVER_assume(array || (array_len == 0));

    /* allocate and initialize c_str (must have at least array_len+1 bytes) */
    size_t c_str_buf_len = array_len + 1;
    char *c_str = malloc(c_str_buf_len);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < c_str_buf_len; ++i) {
        c_str[i] = nondet_char();
    }
    __CPROVER_assume(c_str); /* non‑NULL as required */

    /*--- Save copies for frame condition checks ---*/
    uint8_t *array_copy = NULL;
    if (array_len > 0) {
        array_copy = malloc(array_len);
        __CPROVER_assume(array_copy != NULL);
        memcpy(array_copy, array_buf, array_len);
    }
    char *c_str_copy = malloc(c_str_buf_len);
    __CPROVER_assume(c_str_copy != NULL);
    memcpy(c_str_copy, c_str, c_str_buf_len);

    /*--- Call function under test ---*/
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /*--- Compute expected result ---*/
    bool expected = true;
    for (size_t i = 0; i < array_len; ++i) {
        uint8_t s = (uint8_t)c_str[i];
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
        expected = ((uint8_t)c_str[array_len] == '\0');
    }

    /*--- Postcondition 1: return value correctness ---*/
    assert(result == expected);

    /*--- Postcondition 2: frame condition (inputs unchanged) ---*/
    if (array_len > 0) {
        assert(memcmp(array_buf, array_copy, array_len) == 0);
    }
    assert(memcmp(c_str, c_str_copy, c_str_buf_len) == 0);

    /*--- Cleanup ---*/
    free(array_buf);
    free(array_copy);
    free(c_str);
    free(c_str_copy);

    return 0;
}
