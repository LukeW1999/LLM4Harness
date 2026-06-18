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
    /* 1. Declare and bound inputs */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
        for (size_t i = 0; i < array_len; ++i) {
            array[i] = nondet_uint8_t();
        }
    }

    /* c_str must be a null‑terminated string with at least one byte */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len > 0);
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    char *c_str = malloc(c_str_len);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < c_str_len - 1; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[c_str_len - 1] = '\0';

    /* 2. Save old state */
    struct store_byte_from_buffer array_store;
    if (array_len > 0) {
        save_byte_from_array(array, array_len, &array_store);
    }
    struct store_byte_from_buffer cstr_store;
    save_byte_from_array((const uint8_t *)c_str, c_str_len, &cstr_store);

    /* 3. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Compute expected result */
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

    /* 5. Assert postconditions */
    assert(result == expected);

    /* Unchanged inputs */
    assert_byte_from_buffer_matches(array, &array_store);
    assert_byte_from_buffer_matches((const uint8_t *)c_str, &cstr_store);
    assert(array_len == array_len);
    assert(c_str_len == c_str_len);
    assert(array == array);
    assert(c_str == c_str);
}
