#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_array_eq_c_str_harness() {
    /* 1. Bound array_len */
    size_t array_len;
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    /* 2. Set up array (may be NULL if array_len == 0) */
    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        if (array == NULL) __CPROVER_assume(0);
        for (size_t i = 0; i < array_len; i++) {
            array[i] = nondet_uint8_t();
        }
    }

    /* 3. Set up c_str as a valid null-terminated string */
    size_t str_len;
    __CPROVER_assume(str_len < MAX_BUFFER_SIZE);
    char *c_str = malloc(str_len + 1);
    if (c_str == NULL) __CPROVER_assume(0);
    for (size_t i = 0; i < str_len; i++) {
        c_str[i] = nondet_uint8_t();
        __CPROVER_assume(c_str[i] != '\0'); /* no nulls before terminator */
    }
    c_str[str_len] = '\0';

    /* 4. Save copies for immutability check */
    uint8_t *array_copy = NULL;
    if (array_len > 0) {
        array_copy = malloc(array_len);
        if (array_copy == NULL) __CPROVER_assume(0);
        memcpy(array_copy, array, array_len);
    }
    char *c_str_copy = malloc(str_len + 1);
    if (c_str_copy == NULL) __CPROVER_assume(0);
    memcpy(c_str_copy, c_str, str_len + 1);

    /* 5. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 6. Compute expected result */
    bool expected = true;
    if (array_len == 0) {
        expected = (c_str[0] == '\0');
    } else {
        for (size_t i = 0; i < array_len; i++) {
            if (c_str[i] == '\0' || array[i] != (uint8_t)c_str[i]) {
                expected = false;
                break;
            }
        }
        if (expected) {
            expected = (c_str[array_len] == '\0');
        }
    }

    /* 7. Assert equivalence */
    assert(result == expected);

    /* 8. Immutability checks */
    if (array_len > 0) {
        assert(memcmp(array, array_copy, array_len) == 0);
        free(array_copy);
    }
    assert(memcmp(c_str, c_str_copy, str_len + 1) == 0);
    free(c_str_copy);

    /* 9. Free allocated memory */
    if (array_len > 0) {
        free(array);
    }
    free(c_str);
}
