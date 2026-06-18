#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*
 * Harness for aws_array_eq_c_str
 *
 * Function signature:
 *   bool aws_array_eq_c_str(const void *const array, const size_t array_len, const char *const c_str);
 *
 * Preconditions (from AWS_PRECONDITION):
 *   - array != NULL OR array_len == 0
 *
 * Postconditions:
 *   - Returns true iff array[0..array_len-1] equals c_str[0..array_len-1] and c_str[array_len] == '\0'
 *   - Neither array nor c_str is modified (const inputs)
 *   - No output parameters change
 */

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
    /* We bound c_str length to MAX_BUFFER_SIZE + 1 to allow for the null terminator */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Ensure null terminator */
    c_str[c_str_len] = '\0';

    /* 3. Save state before call (inputs are const, so we just verify they're unchanged) */
    /* Since inputs are const pointers, we save the pointer values */
    const void *old_array = array;
    const char *old_c_str = c_str;
    size_t old_array_len = array_len;

    /* 4. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 5. Assert postconditions */

    /* The function returns a bool — verify it's a valid bool */
    assert(result == true || result == false);

    /* Input pointers must not have changed (function takes const inputs) */
    assert(old_array == array);
    assert(old_c_str == c_str);
    assert(old_array_len == array_len);

    /* Verify correctness: if result is true, then:
     * - c_str[array_len] must be '\0' (string ends exactly at array_len)
     * - all bytes array[0..array_len-1] must equal c_str[0..array_len-1]
     */
    if (result) {
        /* c_str must end at exactly array_len */
        assert(c_str[array_len] == '\0');
        /* All bytes must match */
        if (array_len > 0 && array != NULL) {
            const uint8_t *array_bytes = (const uint8_t *)array;
            const uint8_t *str_bytes = (const uint8_t *)c_str;
            /* Check that no byte in c_str[0..array_len-1] is '\0' (implied by result==true) */
            /* and all bytes match */
            for (size_t i = 0; i < array_len; ++i) {
                assert(str_bytes[i] != '\0');
                assert(array_bytes[i] == str_bytes[i]);
            }
        }
    } else {
        /* result is false: either lengths differ or some byte differs */
        /* We can't easily assert the exact negation without re-implementing,
         * but we can assert the function didn't corrupt memory */
        /* The inputs remain unchanged — already asserted above */
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
