#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void aws_array_eq_c_str_harness(void) {
    /* 1. Set up array input */
    size_t array_len;
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
    }
    /* Precondition: array != NULL OR array_len == 0 */
    __CPROVER_assume(array != NULL || array_len == 0);

    /* 2. Set up c_str input — must be a valid null-terminated string */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Ensure null terminator */
    c_str[c_str_len] = '\0';

    /* 3. Save state before call */
    const void *old_array = array;
    const char *old_c_str = c_str;
    size_t old_array_len = array_len;

    /* 4. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 5. Assert postconditions */

    /* The function returns a bool */
    assert(result == true || result == false);

    /* Input pointers must not have changed */
    assert(old_array == array);
    assert(old_c_str == c_str);
    assert(old_array_len == array_len);

    /* Verify correctness */
    if (result) {
        /* c_str must end at exactly array_len */
        assert(c_str[array_len] == '\0');
        /* All bytes must match */
        if (array_len > 0 && array != NULL) {
            const uint8_t *array_bytes = (const uint8_t *)array;
            const uint8_t *str_bytes = (const uint8_t *)c_str;
            for (size_t i = 0; i < array_len; ++i) {
                assert(str_bytes[i] != '\0');
                assert(array_bytes[i] == str_bytes[i]);
            }
        }
    }

    /* Edge case: if array_len == 0, result is true iff c_str[0] == '\0' */
    if (array_len == 0) {
        if (c_str[0] == '\0') {
            assert(result == true);
        } else {
            assert(result == false);
        }
    }
}

void aws_array_eq_c_str_harness(void) {
    aws_array_eq_c_str_harness();
    return 0;
}
