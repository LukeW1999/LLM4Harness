#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_eq_c_str_harness(void) {
    /* 1. Non‑deterministic inputs, bounded */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    /* Allocate and initialize the array (may be NULL when length is zero) */
    const uint8_t *array = NULL;
    uint8_t *array_old = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
        for (size_t i = 0; i < array_len; ++i) {
            ((uint8_t *)array)[i] = nondet_uint8_t();
        }
        /* Save a copy for immutability check */
        array_old = malloc(array_len);
        __CPROVER_assume(array_old != NULL);
        for (size_t i = 0; i < array_len; ++i) {
            array_old[i] = ((uint8_t *)array)[i];
        }
    }

    /* Allocate and initialize the C‑string.
       It must be at least array_len + 1 bytes long so that the function can read
       c_str[array_len] safely. */
    size_t c_str_buf_len = array_len + 1 + nondet_size_t();
    __CPROVER_assume(c_str_buf_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_buf_len);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < c_str_buf_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    /* Ensure null‑termination exactly at position array_len (the function only checks this) */
    c_str[array_len] = '\0';

    /* Save a copy of the bytes that the function may read */
    char *c_str_old = malloc(array_len + 1);
    __CPROVER_assume(c_str_old != NULL);
    for (size_t i = 0; i < array_len + 1; ++i) {
        c_str_old[i] = c_str[i];
    }

    /* 2. Call the function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 3. Compute the expected result according to the specification */
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
        expected = (c_str[array_len] == '\0');
    }

    /* 4. Post‑condition: return value matches the specification */
    assert(result == expected);

    /* 5. Immutability: inputs must not be modified */
    if (array_len > 0) {
        assert_bytes_match(array, array_old, array_len);
    }
    assert_bytes_match((const uint8_t *)c_str, (const uint8_t *)c_str_old, array_len + 1);
}
