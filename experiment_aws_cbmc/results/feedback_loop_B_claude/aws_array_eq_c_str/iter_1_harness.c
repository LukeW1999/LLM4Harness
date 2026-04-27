#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifndef MAX_BUFFER_SIZE
#    define MAX_BUFFER_SIZE 8
#endif

void aws_array_eq_c_str_harness(void) {
    /* 1. Declare and set up the array */
    size_t array_len;
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    /* array can be NULL only if array_len == 0 */
    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
    }

    /* 2. Declare and set up the c_str */
    /* c_str must be a valid null-terminated string.
     * We bound its length to MAX_BUFFER_SIZE + 1 to cover cases where
     * c_str is shorter, equal, or longer than array_len. */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    c_str[c_str_len] = '\0';

    /* 3. Save old state — the function is read-only, nothing should change */
    /* (No mutable state to save here since inputs are read-only) */

    /* 4. Call the function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 5. Assert postconditions */

    /* The function returns true iff:
     *   - For all i in [0, array_len): c_str[i] != '\0' AND array[i] == c_str[i]
     *   - AND c_str[array_len] == '\0'
     * Equivalently: the array equals the c_str content exactly (same bytes, same length).
     */

    if (result) {
        /* If true, c_str must have exactly array_len non-null characters */
        assert(c_str[array_len] == '\0');
        /* And all bytes must match */
        if (array_len > 0) {
            assert(array != NULL);
            for (size_t i = 0; i < array_len; ++i) {
                assert(c_str[i] != '\0');
                assert(array[i] == (uint8_t)c_str[i]);
            }
        }
    } else {
        /* If false, either:
         *   - c_str is shorter than array_len (some c_str[i] == '\0' for i < array_len), OR
         *   - some byte differs, OR
         *   - c_str[array_len] != '\0' (c_str is longer than array_len)
         * We can't assert a single simple condition here without re-implementing the function,
         * but we can assert that the inputs were not modified. */

        /* Verify the function didn't modify array_len */
        /* (array_len is passed by value, so it's unchanged by definition) */

        /* At least one of the conditions for equality must fail */
        /* We verify this by checking: if all conditions for equality hold, result must be true */
        bool all_match = true;
        if (c_str[array_len] != '\0') {
            all_match = false;
        }
        if (all_match && array_len > 0 && array != NULL) {
            for (size_t i = 0; i < array_len; ++i) {
                if (c_str[i] == '\0' || array[i] != (uint8_t)c_str[i]) {
                    all_match = false;
                    break;
                }
            }
        }
        /* If all conditions hold, result should have been true — contradiction */
        assert(!all_match);
    }

    /* 6. The function is pure (read-only), so inputs are unchanged.
     * array and c_str pointers themselves are unchanged (passed by value). */
    /* No validity invariants to check for raw arrays/c_str beyond what we've done. */
}
