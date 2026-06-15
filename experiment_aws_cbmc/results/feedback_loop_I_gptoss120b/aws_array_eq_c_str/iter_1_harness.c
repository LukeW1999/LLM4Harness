#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_eq_c_str_harness(void) {
    /* 1. nondeterministic inputs */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    const uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
        /* make array contents nondeterministic */
        for (size_t i = 0; i < array_len; ++i) {
            ((uint8_t *)array)[i] = nondet_uint8_t();
        }
    }

    /* c_str must be a valid null‑terminated string.
       Allocate at least array_len+1 bytes so the function can read
       c_str[array_len] safely. */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len >= array_len);
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);               /* +1 for guaranteed terminator */
    __CPROVER_assume(c_str != NULL);

    /* Choose a nondeterministic position for the terminating '\0' */
    size_t null_pos = nondet_size_t();
    __CPROVER_assume(null_pos <= c_str_len);
    for (size_t i = 0; i <= c_str_len; ++i) {
        if (i == null_pos) {
            c_str[i] = '\0';
        } else {
            c_str[i] = (char)nondet_uint8_t();
        }
    }

    /* 2. Save old state */
    uint8_t *old_array = NULL;
    if (array_len > 0) {
        old_array = malloc(array_len);
        __CPROVER_assume(old_array != NULL);
        memcpy(old_array, array, array_len);
    }
    char *old_c_str = malloc(c_str_len + 1);
    __CPROVER_assume(old_c_str != NULL);
    memcpy(old_c_str, c_str, c_str_len + 1);

    /* 3. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Compute the specification condition */
    bool spec_true = true;
    for (size_t i = 0; i < array_len; ++i) {
        if ((unsigned char)c_str[i] == '\0') {
            spec_true = false;
            break;
        }
        if (array_len > 0 && array[i] != (uint8_t)c_str[i]) {
            spec_true = false;
            break;
        }
    }
    if (spec_true) {
        if ((unsigned char)c_str[array_len] != '\0') {
            spec_true = false;
        }
    }

    /* 5. Assert post‑conditions */
    /* result must match the specification */
    assert(result == spec_true);

    /* array contents must be unchanged */
    if (array_len > 0) {
        for (size_t i = 0; i < array_len; ++i) {
            assert(((uint8_t *)array)[i] == old_array[i]);
        }
    }

    /* c_str contents must be unchanged */
    for (size_t i = 0; i <= c_str_len; ++i) {
        assert(c_str[i] == old_c_str[i]);
    }

    /* 6. Clean up */
    free((void *)array);
    free(old_array);
    free(c_str);
    free(old_c_str);
}
