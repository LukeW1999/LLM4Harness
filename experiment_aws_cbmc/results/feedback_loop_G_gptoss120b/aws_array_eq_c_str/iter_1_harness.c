/* Harness for aws_array_eq_c_str */
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "aws/common/byte_buf.h"
#include "proof_helpers/make_common_data_structures.h"

/* MAX_BUFFER_SIZE is provided by the build system */
void aws_array_eq_c_str_harness(void) {
    /* 1. Non‑deterministic inputs */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    uint8_t *array_bytes = NULL;
    if (array_len > 0) {
        array_bytes = malloc(array_len);
        __CPROVER_assume(array_bytes != NULL);
        for (size_t i = 0; i < array_len; ++i) {
            array_bytes[i] = nondet_uint8_t();
        }
    }
    const void *array = array_bytes;               /* const pointer as required by API */

    /* c_str must be readable for at least array_len + 1 bytes */
    size_t c_str_len = array_len + 1;
    char *c_str_buf = malloc(c_str_len);
    __CPROVER_assume(c_str_buf != NULL);
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str_buf[i] = (char)nondet_uint8_t();
    }
    const char *c_str = c_str_buf;

    /* 2. Save old state (inputs are immutable) */
    const void *old_array   = array;
    size_t       old_len    = array_len;
    const char  *old_c_str  = c_str;

    /* 3. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Unchanged fields – inputs must stay the same */
    assert(array   == old_array);
    assert(array_len == old_len);
    assert(c_str   == old_c_str);

    /* 5. Post‑conditions */
    if (result) {
        /* On success the contents must be equal and the string must be NUL‑terminated */
        for (size_t i = 0; i < array_len; ++i) {
            assert(((const uint8_t *)array)[i] == (uint8_t)c_str[i]);
        }
        assert(c_str[array_len] == '\0');
    } else {
        /* On failure the “all‑match” condition must be false */
        bool all_match = true;
        for (size_t i = 0; i < array_len; ++i) {
            if (((const uint8_t *)array)[i] != (uint8_t)c_str[i]) {
                all_match = false;
                break;
            }
        }
        if (all_match) {
            all_match = (c_str[array_len] == '\0');
        }
        assert(!all_match);
    }

    /* 6. No additional validity invariants for raw pointers */
}
