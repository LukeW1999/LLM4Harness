#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_eq_c_str_harness(void) {
    /* 1. Nondeterministic inputs */
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

    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[c_str_len] = '\0';

    /* 2. Precondition from the implementation */
    __CPROVER_assume(array != NULL || array_len == 0);

    /* 3. Save old state for immutability checks */
    struct store_byte_from_buffer old_array;
    if (array_len > 0) {
        save_byte_from_array(array, array_len, &old_array);
    }
    struct store_byte_from_buffer old_cstr;
    save_byte_from_array((uint8_t *)c_str, c_str_len + 1, &old_cstr);

    /* 4. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 5. Compute expected result according to specification */
    bool expected = true;
    for (size_t i = 0; i < array_len; ++i) {
        uint8_t s = (uint8_t)c_str[i];
        if (s == '\0' || array[i] != s) {
            expected = false;
            break;
        }
    }
    if (expected) {
        if (c_str[array_len] != '\0') {
            expected = false;
        }
    }

    /* 6. Assert postconditions */
    assert(result == expected);

    /* 7. Assert inputs unchanged */
    if (array_len > 0) {
        assert_byte_from_buffer_matches(array, &old_array);
    }
    assert_byte_from_buffer_matches((uint8_t *)c_str, &old_cstr);
}
