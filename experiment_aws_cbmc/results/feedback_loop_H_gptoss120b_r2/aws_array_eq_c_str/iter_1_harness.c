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
    /* 1. Declare inputs */
    const void *array;
    size_t array_len;
    const char *c_str;

    /* 2. Non‑deterministically choose lengths bounded by MAX_BUFFER_SIZE */
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    /* 3. Allocate memory for array (if length > 0) */
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
    } else {
        /* When length is zero the pointer may be NULL or any value */
        array = nondet_bool() ? NULL : malloc(1);
    }

    /* 4. Allocate memory for c_str (always at least one byte for the terminator) */
    c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Fill c_str with nondet data and ensure null‑termination */
    for (size_t i = 0; i < c_str_len; ++i) {
        ((uint8_t *)c_str)[i] = nondet_uint8_t();
    }
    ((uint8_t *)c_str)[c_str_len] = '\0';

    /* 5. Save old state for immutability checks */
    const void *old_array = array;
    const char *old_c_str = c_str;
    struct store_byte_from_buffer old_array_store;
    struct store_byte_from_buffer old_c_str_store;
    if (array_len > 0) {
        save_byte_from_array((const uint8_t *)array, array_len, &old_array_store);
    }
    save_byte_from_array((const uint8_t *)c_str, c_str_len + 1, &old_c_str_store);

    /* 6. Enforce function precondition */
    __CPROVER_assume(array != NULL || array_len == 0);

    /* 7. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 8. Compute expected result using the specification */
    bool expected = true;
    for (size_t i = 0; i < array_len; ++i) {
        uint8_t s = (uint8_t)c_str[i];
        if (s == '\0') {
            expected = false;
            break;
        }
        if (((const uint8_t *)array)[i] != s) {
            expected = false;
            break;
        }
    }
    if (expected) {
        expected = (c_str[array_len] == '\0');
    }

    /* 9. Assert postconditions */
    assert(result == expected);

    /* 10. Assert inputs unchanged */
    assert(array == old_array);
    assert(c_str == old_c_str);
    if (array_len > 0) {
        assert_byte_from_buffer_matches((const uint8_t *)array, &old_array_store);
    }
    assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_c_str_store);
}
