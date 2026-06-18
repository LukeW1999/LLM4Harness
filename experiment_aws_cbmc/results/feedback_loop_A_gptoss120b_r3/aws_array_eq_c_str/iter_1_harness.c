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
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_eq_c_str_harness(void) {
    /* 1. Non‑deterministic inputs, bounded */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    const void *array = NULL;
    uint8_t *array_copy = NULL;
    if (array_len > 0) {
        uint8_t *buf = malloc(array_len);
        __CPROVER_assume(buf != NULL);
        /* fill with nondet bytes */
        for (size_t i = 0; i < array_len; ++i) {
            buf[i] = nondet_uint8_t();
        }
        array = buf;

        /* keep a copy for later immutability check */
        array_copy = malloc(array_len);
        __CPROVER_assume(array_copy != NULL);
        memcpy(array_copy, buf, array_len);
    }

    /* c_str: allocate a buffer that is guaranteed to be null‑terminated */
    size_t c_str_buf_len = nondet_size_t();
    __CPROVER_assume(c_str_buf_len <= MAX_BUFFER_SIZE);
    char *c_str_buf = malloc(c_str_buf_len + 1);
    __CPROVER_assume(c_str_buf != NULL);
    for (size_t i = 0; i < c_str_buf_len; ++i) {
        c_str_buf[i] = (char)nondet_uint8_t();
    }
    c_str_buf[c_str_buf_len] = '\0'; /* ensure termination */

    /* keep a copy for immutability check */
    char *c_str_copy = malloc(c_str_buf_len + 1);
    __CPROVER_assume(c_str_copy != NULL);
    memcpy(c_str_copy, c_str_buf, c_str_buf_len + 1);

    /* 2. Precondition: either array is non‑NULL or length is zero */
    __CPROVER_assume(array != NULL || array_len == 0);
    /* c_str must be non‑NULL (function dereferences it) */
    __CPROVER_assume(c_str_buf != NULL);

    /* 3. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str_buf);

    /* 4. Compute expected result independently */
    bool expected = true;
    const uint8_t *array_bytes = (const uint8_t *)array;
    const uint8_t *str_bytes   = (const uint8_t *)c_str_buf;

    for (size_t i = 0; i < array_len; ++i) {
        uint8_t s = str_bytes[i];
        if (s == '\0' || array_bytes[i] != s) {
            expected = false;
            break;
        }
    }
    if (expected) {
        expected = (str_bytes[array_len] == '\0');
    }

    /* 5. Assert that the return value matches the specification */
    assert(result == expected);

    /* 6. Assert that inputs are unchanged */
    if (array_len > 0) {
        assert_bytes_match((const uint8_t *)array, (const uint8_t *)array_copy, array_len);
    }
    assert_bytes_match((const uint8_t *)c_str_buf, (const uint8_t *)c_str_copy, c_str_buf_len + 1);

    /* 7. Clean up */
    free((void *)array);
    free(array_copy);
    free(c_str_buf);
    free(c_str_copy);
}
