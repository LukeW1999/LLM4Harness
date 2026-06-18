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
    /* 1. Declare inputs and bound them */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
    }

    /* c_str must be a null‑terminated string with at least array_len+1 readable bytes */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len > 0);
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str_len >= array_len + 1);

    char *c_str = malloc(c_str_len);
    __CPROVER_assume(c_str != NULL);
    c_str[c_str_len - 1] = '\0';               /* ensure termination */

    /* 2. Save old state for immutability checks */
    uint8_t *old_array_ptr = array;
    size_t   old_array_len = array_len;
    char    *old_c_str_ptr = c_str;

    struct store_byte_from_buffer old_array_store;
    save_byte_from_array(array, array_len, &old_array_store);

    struct store_byte_from_buffer old_cstr_store;
    save_byte_from_array((const uint8_t *)c_str, c_str_len, &old_cstr_store);

    /* 3. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Compute expected result using the specification */
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

    /* 5. Assert postconditions */
    assert(result == expected);                                   /* return matches spec */

    /* unchanged pointers and lengths */
    assert(array == old_array_ptr);
    assert(array_len == old_array_len);
    assert(c_str == old_c_str_ptr);

    /* unchanged contents */
    assert_byte_from_buffer_matches(array, &old_array_store);
    assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_cstr_store);
}
