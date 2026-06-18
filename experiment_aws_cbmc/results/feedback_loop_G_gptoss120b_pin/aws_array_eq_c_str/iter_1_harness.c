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
    /* 1. Nondeterministic inputs */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str_len >= array_len); /* ensure c_str has enough bytes for the check */

    /* Allocate and initialize array */
    uint8_t *array_bytes = NULL;
    if (array_len > 0) {
        array_bytes = malloc(array_len);
        __CPROVER_assume(array_bytes != NULL);
        /* nondet fill */
        for (size_t i = 0; i < array_len; ++i) {
            array_bytes[i] = nondet_uint8_t();
        }
    }
    const void *array = (const void *)array_bytes; /* may be NULL when array_len == 0 */

    /* Allocate and initialize c_str (including null terminator) */
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[c_str_len] = '\0';

    /* 2. Save old state */
    uint8_t *old_array_copy = NULL;
    if (array_len > 0) {
        old_array_copy = malloc(array_len);
        __CPROVER_assume(old_array_copy != NULL);
        for (size_t i = 0; i < array_len; ++i) {
            old_array_copy[i] = array_bytes[i];
        }
    }

    char *old_c_str_copy = malloc(c_str_len + 1);
    __CPROVER_assume(old_c_str_copy != NULL);
    for (size_t i = 0; i <= c_str_len; ++i) {
        old_c_str_copy[i] = c_str[i];
    }

    const void *old_array = array;
    size_t old_array_len = array_len;
    const char *old_c_str = c_str;

    /* 3. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Compute expected result according to specification */
    bool expected = true;
    for (size_t i = 0; i < array_len; ++i) {
        uint8_t s = (uint8_t) c_str[i];
        if (s == '\0') {
            expected = false;
            break;
        }
        if (array_bytes[i] != s) {
            expected = false;
            break;
        }
    }
    if (expected) {
        expected = (c_str[array_len] == '\0');
    }

    /* 5. Assert postconditions */
    assert(result == expected);

    /* Unchanged inputs */
    assert(array == old_array);
    assert(array_len == old_array_len);
    assert(c_str == old_c_str);

    if (array_len > 0) {
        assert_bytes_match(old_array_copy, (const uint8_t *)array_bytes, array_len);
    }
    assert_bytes_match((const uint8_t *)old_c_str_copy,
                       (const uint8_t *)c_str,
                       c_str_len + 1);

    /* 6. Clean up */
    free(old_array_copy);
    free(old_c_str_copy);
    free(array_bytes);
    free(c_str);
}
