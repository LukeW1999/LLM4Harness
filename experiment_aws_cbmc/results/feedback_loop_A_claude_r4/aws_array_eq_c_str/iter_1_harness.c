#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void aws_array_eq_c_str_harness(void) {
    /* 1. Set up the array */
    size_t array_len;
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
    }

    /* 2. Set up the c_str (null-terminated string) */
    /* c_str length can be up to MAX_BUFFER_SIZE + 1 to allow for various comparison outcomes */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Ensure null terminator at end */
    c_str[c_str_len] = '\0';
    /* c_str contents are non-deterministic (already set by malloc + CBMC) */
    /* Ensure no null bytes before c_str_len to make it a valid C string of length c_str_len */
    for (size_t i = 0; i < c_str_len; i++) {
        __CPROVER_assume(c_str[i] != '\0');
    }

    /* 3. Call the function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Assert postconditions */
    /* The function returns true iff:
     * - array_len == strlen(c_str) (i.e., c_str_len == array_len)
     * - AND all bytes match
     * - AND c_str does not contain null terminator within the first array_len bytes
     */

    /* If result is true, then array_len must equal c_str_len */
    if (result) {
        assert(array_len == c_str_len);
        /* Also verify that the bytes match */
        if (array_len > 0) {
            assert_bytes_match(array, (const uint8_t *)c_str, array_len);
        }
    }

    /* If array_len != c_str_len, result must be false */
    if (array_len != c_str_len) {
        assert(!result);
    }

    /* The function is pure (no side effects on inputs) */
    /* array and c_str are not modified by the function */
    /* No validity invariants to check for scalar types */
}
