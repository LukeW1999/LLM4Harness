#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "aws/common/byte_buf.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_array_eq_c_str_harness(void) {
    /* 1. nondeterministic lengths, bounded */
    size_t array_len = nondet_size_t();
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    /* 2. allocate and initialize array */
    const uint8_t *array = NULL;
    uint8_t *array_copy = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
        /* fill with nondet data */
        for (size_t i = 0; i < array_len; ++i) {
            ((uint8_t *)array)[i] = nondet_uint8_t();
        }
        /* keep a copy for immutability check */
        array_copy = malloc(array_len);
        __CPROVER_assume(array_copy != NULL);
        memcpy(array_copy, array, array_len);
    }

    /* 3. allocate and initialize c_str (null‑terminated) */
    char *c_str = NULL;
    char *c_str_copy = NULL;
    c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[c_str_len] = '\0'; /* ensure termination */

    c_str_copy = malloc(c_str_len + 1);
    __CPROVER_assume(c_str_copy != NULL);
    memcpy(c_str_copy, c_str, c_str_len + 1);

    /* 4. precondition from the implementation */
    __CPROVER_assume(array != NULL || array_len == 0);

    /* 5. call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 6. compute expected result using the same specification */
    bool expected = true;
    for (size_t i = 0; i < array_len; ++i) {
        uint8_t s = (uint8_t)c_str[i];
        if (s == '\0' || array[i] != s) {
            expected = false;
            break;
        }
    }
    if (expected) {
        expected = (c_str[array_len] == '\0');
    }

    /* 7. post‑condition: return value matches specification */
    assert(result == expected);

    /* 8. immutability: inputs must not be modified */
    if (array_len > 0) {
        assert_bytes_match(array, array_copy, array_len);
    }
    assert_bytes_match((const uint8_t *)c_str, (const uint8_t *)c_str_copy, c_str_len + 1);
}
