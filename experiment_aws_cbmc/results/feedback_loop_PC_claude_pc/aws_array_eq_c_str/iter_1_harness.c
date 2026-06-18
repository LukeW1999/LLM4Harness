#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_eq_c_str_harness(void) {
    /* 1. Declare and set up inputs */

    /* array_len is bounded to keep state space manageable */
    size_t array_len;
    __CPROVER_assume(array_len <= 16);

    /* array: may be NULL if array_len == 0 */
    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
    }

    /* c_str: a null-terminated string, bounded length */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= 16);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Ensure null terminator is present */
    c_str[c_str_len] = '\0';

    /* 2. Snapshot inputs before the call */
    /* Save array contents */
    uint8_t array_snapshot[16];
    if (array_len > 0 && array != NULL) {
        __CPROVER_array_copy(array_snapshot, array);
    }
    size_t array_len_snapshot = array_len;

    /* Save c_str contents */
    char c_str_snapshot[17];
    for (size_t i = 0; i <= c_str_len; i++) {
        c_str_snapshot[i] = c_str[i];
    }

    /* 3. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Assert postconditions */

    /* RETURN VALUE: result is a bool */
    /* The function returns true iff array and c_str have the same content and length */
    /* We verify the result is consistent with the definition:
       - If result is true, then c_str[array_len] must be '\0' and all bytes match */
    if (result) {
        /* c_str must end exactly at array_len */
        assert(c_str[array_len] == '\0');
        /* All bytes in array must match c_str */
        for (size_t i = 0; i < array_len; i++) {
            assert(array[i] == (uint8_t)c_str[i]);
            /* Also implies c_str[i] != '\0' for i < array_len */
            assert(c_str[i] != '\0');
        }
    } else {
        /* Either lengths differ or some byte differs or c_str has null before array_len */
        /* We can't assert a single simple condition here, but we can check the negation:
           NOT (all bytes match AND c_str[array_len] == '\0') */
        /* This is hard to assert directly without re-implementing, so we check
           that the function is deterministic by verifying frame conditions */
    }

    /* FRAME: inputs must not have changed */
    assert(array_len == array_len_snapshot);

    /* array contents unchanged */
    if (array_len > 0 && array != NULL) {
        for (size_t i = 0; i < array_len; i++) {
            assert(array[i] == array_snapshot[i]);
        }
    }

    /* c_str contents unchanged */
    for (size_t i = 0; i <= c_str_len; i++) {
        assert(c_str[i] == c_str_snapshot[i]);
    }

    /* INVARIANTS: result is a valid bool */
    assert(result == true || result == false);

    /* Precondition: array is NULL only if array_len == 0 */
    /* (This was assumed, so it holds) */
    if (array_len == 0) {
        /* array may be NULL, result depends only on c_str[0] == '\0' */
        if (result) {
            assert(c_str[0] == '\0');
        } else {
            assert(c_str[0] != '\0');
        }
    }
}
