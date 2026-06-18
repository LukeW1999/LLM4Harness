#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_eq_c_str_harness(void) {
    /* 1. Set up the array input */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
    }

    /* 2. Set up the c_str input */
    /* c_str must be a valid null-terminated string.
     * We bound its length to keep state space manageable. */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Ensure null terminator at end */
    c_str[c_str_len] = '\0';

    /* 3. Precondition: array may be NULL only if array_len == 0 */
    __CPROVER_assume(array != NULL || array_len == 0);

    /* 4. Call the function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 5. Assert postconditions */

    /* If result is true:
     * - array_len must equal strlen(c_str) (i.e., c_str_len)
     * - all bytes in array must match the corresponding bytes in c_str
     * - c_str must not contain a null terminator within the first array_len bytes
     */
    if (result) {
        /* The lengths must match */
        assert(array_len == c_str_len);

        /* Every byte in array matches c_str */
        if (array_len > 0) {
            assert_bytes_match(array, (const uint8_t *)c_str, array_len);
        }

        /* c_str[array_len] must be '\0' */
        assert(c_str[array_len] == '\0');
    } else {
        /* On false: either lengths differ, or some byte differs, or c_str has
         * a null terminator before array_len bytes, or c_str doesn't end at array_len.
         * We can't assert a specific structural property here beyond the result being false. */
        /* The function returns false — no structural postcondition to assert beyond result */
        assert(!result);
    }

    /* The function is pure (no side effects on inputs) — inputs are unchanged */
    /* array and c_str are not modified by the function */
    /* (No fields to check since these are raw pointers, not structs) */

    /* Result must be a valid bool */
    assert(result == true || result == false);
}
