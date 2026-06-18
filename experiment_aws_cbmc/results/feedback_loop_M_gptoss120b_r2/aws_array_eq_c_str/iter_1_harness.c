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
#include <string.h>

void aws_array_eq_c_str_harness(void) {
    /* 1. Non‑deterministic inputs bounded */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    uint8_t *array;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
        for (size_t i = 0; i < array_len; ++i) {
            array[i] = nondet_uint8_t();
        }
    } else {
        array = NULL;
    }

    char *c_str = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        c_str[i] = (char)nondet_uint8_t();      /* nondet characters */
    }
    c_str[MAX_BUFFER_SIZE - 1] = '\0';         /* guarantee null‑termination */

    /* 2. Save old state */
    uint8_t *old_array = NULL;
    if (array_len > 0) {
        old_array = malloc(array_len);
        __CPROVER_assume(old_array != NULL);
        memcpy(old_array, array, array_len);
    }
    char *old_c_str = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(old_c_str != NULL);
    memcpy(old_c_str, c_str, MAX_BUFFER_SIZE);

    /* 3. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Assert inputs unchanged */
    if (array_len > 0) {
        for (size_t i = 0; i < array_len; ++i) {
            assert(array[i] == old_array[i]);
        }
    }
    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        assert(c_str[i] == old_c_str[i]);
    }

    /* 5. Post‑condition assertions */
    if (result) {
        /* success: all bytes equal and null terminator exactly after array_len */
        for (size_t i = 0; i < array_len; ++i) {
            assert((uint8_t)c_str[i] == array[i]);
        }
        assert(c_str[array_len] == '\0');
    } else {
        /* failure: either a mismatching byte or missing null terminator */
        bool mismatch = false;
        for (size_t i = 0; i < array_len; ++i) {
            if ((uint8_t)c_str[i] != array[i]) {
                mismatch = true;
                break;
            }
        }
        bool null_missing = c_str[array_len] != '\0';
        assert(mismatch || null_missing);
    }
}
