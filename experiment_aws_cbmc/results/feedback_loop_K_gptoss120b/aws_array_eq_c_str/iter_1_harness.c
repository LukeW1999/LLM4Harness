/*=== Contract for aws_array_eq_c_str =============================

Preconditions:
  1. (array != NULL) || (array_len == 0)
  2. c_str != NULL
  3. The memory region pointed to by 'array' contains at least 'array_len' bytes.
  4. The memory region pointed to by 'c_str' is a valid C‑string (i.e., it
     contains a terminating '\0' somewhere at or after index 'array_len').

Postconditions (validity):
  * The function returns true  ⇔  the first 'array_len' bytes of 'c_str' are
    defined, none of them is '\0', they are byte‑wise equal to the contents of
    'array', and the byte immediately following those 'array_len' bytes is
    the terminating '\0'.
  * The function returns false otherwise.

Postconditions (frame):
  * The contents of the memory regions pointed to by 'array' and 'c_str' are
    not modified by the call.
  * No other memory is modified.

===================================================================*/

#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

void aws_array_eq_c_str_harness(void) {
    /* nondet size for the array */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= 64);               /* bound for tractability */

    /* allocate array (may be NULL when length is zero) */
    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array!= NULL);
        /* fill with nondet data */
        for (size_t i = 0; i < array_len; ++i) {
            array[i] = nondet_uint8_t();
        }
    }

    /* nondet size for the C string (must be at least array_len) */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len >= array_len);
    __CPROVER_assume(c_str_len <= 64);               /* bound for tractability */

    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* fill with nondet data */
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    /* ensure null‑termination */
    c_str[c_str_len] = '\0';

    /* keep copies for frame condition checks */
    uint8_t *array_copy = NULL;
    if (array_len > 0) {
        array_copy = malloc(array_len);
        __CPROVER_assume(array_copy != NULL);
        memcpy(array_copy, array, array_len);
    }

    char *c_str_copy = malloc(c_str_len + 1);
    __CPROVER_assume(c_str_copy != NULL);
    memcpy(c_str_copy, c_str, c_str_len + 1);

    /* call the function under verification */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* ----- Frame condition: inputs must be unchanged ----- */
    if (array_len > 0) {
        assert(memcmp(array, array_copy, array_len) == 0);
    }
    assert(memcmp(c_str, c_str_copy, c_str_len + 1) == 0);

    /* ----- Functional postcondition ----- */
    bool expected = true;
    for (size_t i = 0; i < array_len; ++i) {
        uint8_t s = (uint8_t)c_str[i];
        if (s == '\0' || array[i] != s) {
            expected = false;
            break;
        }
    }
    if (expected) {
        expected = ((uint8_t)c_str[array_len] == '\0');
    }
    assert(result == expected);

    /* clean up */
    free(array);
    free(array_copy);
    free(c_str);
    free(c_str_copy);
    return 0;
}
