#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* Harness for aws_array_eq_c_str */
void aws_array_eq_c_str_harness(void) {
    /* 1. Nondeterministic inputs, bounded */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    /* 2. Allocate and initialize array */
    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
        for (size_t i = 0; i < array_len; ++i) {
            array[i] = nondet_uint8_t();
        }
    }

    /* 3. Allocate and initialize null‑terminated string */
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
        __CPROVER_assume(c_str[i] != '\0'); /* ensure proper C‑string length */
    }
    c_str[c_str_len] = '\0';

    /* 4. Save old state for immutability checks */
    uint8_t *old_array = NULL;
    if (array_len > 0) {
        old_array = malloc(array_len);
        __CPROVER_assume(old_array != NULL);
        for (size_t i = 0; i < array_len; ++i) {
            old_array[i] = array[i];
        }
    }

    char *old_c_str = malloc(c_str_len + 1);
    __CPROVER_assume(old_c_str != NULL);
    for (size_t i = 0; i <= c_str_len; ++i) {
        old_c_str[i] = c_str[i];
    }

    /* 5. Precondition: array must be non‑NULL if length > 0 */
    __CPROVER_assume(array != NULL || array_len == 0);

    /* 6. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 7. Compute expected result according to specification */
    bool expected = true;
    if (c_str_len != array_len) {
        expected = false;
    } else {
        for (size_t i = 0; i < array_len; ++i) {
            if ((uint8_t)old_c_str[i] != old_array[i]) {
                expected = false;
                break;
            }
        }
    }

    /* 8. Assert that the return value matches the specification */
    assert(result == expected);

    /* 9. Assert that inputs are unchanged */
    if (array_len > 0) {
        for (size_t i = 0; i < array_len; ++i) {
            assert(array[i] == old_array[i]);
        }
    }
    for (size_t i = 0; i <= c_str_len; ++i) {
        assert(c_str[i] == old_c_str[i]);
    }

    /* 10. Clean up */
    free(old_array);
    free(old_c_str);
    free(array);
    free(c_str);
}
