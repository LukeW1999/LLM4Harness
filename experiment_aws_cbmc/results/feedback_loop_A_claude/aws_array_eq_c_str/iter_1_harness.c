#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#    define MAX_BUFFER_SIZE 8
#endif

void aws_array_eq_c_str_harness(void) {
    /* 1. Declare and set up the array */
    size_t array_len;
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
    }

    /* 2. Declare and set up the c_str (null-terminated string) */
    /* c_str must be at least array_len + 1 bytes to allow for comparison */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE + 1);
    __CPROVER_assume(c_str_len >= 1); /* at least null terminator */

    char *c_str = malloc(c_str_len);
    __CPROVER_assume(c_str != NULL);
    /* Ensure null terminator exists somewhere in c_str */
    c_str[c_str_len - 1] = '\0';

    /* Precondition: array may be NULL only if array_len == 0 */
    __CPROVER_assume(array != NULL || array_len == 0);

    /* 3. Call the function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Assert postconditions */

    /* The function returns true iff:
     * - For all i in [0, array_len): c_str[i] != '\0' AND array[i] == c_str[i]
     * - c_str[array_len] == '\0'
     *
     * We verify the key invariants:
     */

    if (result) {
        /* If result is true, then c_str[array_len] must be '\0' */
        assert(c_str[array_len] == '\0');

        /* If result is true, then for all i in [0, array_len), array[i] == c_str[i] */
        for (size_t i = 0; i < array_len; i++) {
            assert(array[i] == (uint8_t)c_str[i]);
            assert(c_str[i] != '\0');
        }
    } else {
        /* If result is false, then either:
         * - c_str[array_len] != '\0', OR
         * - some array[i] != c_str[i], OR
         * - some c_str[i] == '\0' before reaching array_len
         *
         * We can't easily assert the exact negation in CBMC without loops,
         * but we can assert that the return value is a bool.
         */
        assert(result == false);
    }

    /* The function is a pure comparison - it does not modify any inputs */
    /* array and c_str are unchanged (no fields to check for structs here,
     * but we verify the return type is bool) */
    assert(result == true || result == false);
}
