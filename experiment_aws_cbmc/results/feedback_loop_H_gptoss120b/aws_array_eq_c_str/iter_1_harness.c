#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "aws/common/byte_buf.h"
#include "proof_helpers/make_common_data_structures.h"

/* CBMC harness for aws_array_eq_c_str */
void aws_array_eq_c_str_harness(void) {
    /* 1. Nondeterministic inputs */
    size_t array_len = nondet_size_t();
    const uint8_t *array_bytes = NULL;

    /* Allocate array if length > 0 */
    if (array_len > 0) {
        array_bytes = malloc(array_len);
        __CPROVER_assume(array_bytes != NULL);
        /* make the allocated region readable */
        __CPROVER_assume(AWS_MEM_IS_READABLE(array_bytes, array_len));
        /* nondeterministically fill the array */
        for (size_t i = 0; i < array_len; ++i) {
            ((uint8_t *)array_bytes)[i] = nondet_uint8_t();
        }
    }
    const void *array = array_bytes; /* const void * as required by the API */

    /* Allocate a null‑terminated C string */
    size_t str_len = nondet_size_t();               /* length *before* the terminating NUL */
    char *c_str = malloc(str_len + 1);
    __CPROVER_assume(c_str != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, str_len + 1));

    /* Fill the string with nondeterministic bytes; the last byte is forced to NUL */
    for (size_t i = 0; i < str_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[str_len] = '\0';

    /* 2. Save old state (pointers and length) */
    const void *old_array   = array;
    const char *old_c_str   = c_str;
    size_t      old_len     = array_len;

    /* 3. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Post‑condition checks */
    /* Unchanged inputs */
    assert(array   == old_array);
    assert(c_str   == old_c_str);
    assert(array_len == old_len);

    if (result) {
        /* On success the function guarantees:
         *   - every character of the string up to array_len is non‑NUL and equal to the array byte
         *   - the character at position array_len is NUL
         */
        for (size_t i = 0; i < array_len; ++i) {
            assert(c_str[i] != '\0');
            assert(array_bytes[i] == (uint8_t)c_str[i]);
        }
        assert(c_str[array_len] == '\0');
    } else {
        /* On failure at least one of the above conditions must be violated */
        bool violation = false;
        for (size_t i = 0; i < array_len; ++i) {
            if (c_str[i] == '\0' || array_bytes[i] != (uint8_t)c_str[i]) {
                violation = true;
                break;
            }
        }
        if (!violation) {
            violation = (c_str[array_len] != '\0');
        }
        assert(violation);
    }

    /* 5. Validity invariants (readability of the buffers) */
    if (array_len > 0) {
        assert(AWS_MEM_IS_READABLE(array_bytes, array_len));
    }
    assert(AWS_MEM_IS_READABLE(c_str, str_len + 1));
}
