#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Keep bounds small for CBMC tractability */
#define MAX_ARRAY_LEN 4
#define MAX_STR_LEN   (MAX_ARRAY_LEN + 2)

bool aws_array_eq_c_str(const void *const array, const size_t array_len, const char *const c_str);

void aws_array_eq_c_str_harness(void) {
    /* --- Inputs --- */
    size_t array_len;
    __CPROVER_assume(array_len <= MAX_ARRAY_LEN);

    /* array: NULL allowed only when array_len == 0 */
    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
        /* contents are fully non-deterministic (already the case after malloc in CBMC) */
    }

    /* c_str: must be non-NULL and null-terminated, length >= array_len */
    size_t str_len;
    __CPROVER_assume(str_len <= MAX_STR_LEN);
    /* We need c_str to have at least array_len+1 characters accessible.
     * Allocate str_len+1 bytes so there is always a null terminator slot. */
    __CPROVER_assume(str_len >= array_len); /* ensures str_bytes[array_len] is accessible */

    char *c_str = malloc(str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Make contents non-deterministic but ensure null terminator at str_len */
    c_str[str_len] = '\0';
    /* bytes 0..str_len-1 are non-deterministic (CBMC leaves malloc contents symbolic) */

    /* --- Save snapshots for frame conditions --- */
    /* Copy array contents before call */
    uint8_t *array_snapshot = NULL;
    if (array_len > 0) {
        array_snapshot = malloc(array_len);
        __CPROVER_assume(array_snapshot != NULL);
        __CPROVER_array_copy(array_snapshot, array, array_len);
    }

    size_t str_snapshot_len = str_len + 1;
    char *str_snapshot = malloc(str_snapshot_len);
    __CPROVER_assume(str_snapshot != NULL);
    __CPROVER_array_copy(str_snapshot, c_str, str_snapshot_len);

    /* --- Call the function under test --- */
    bool result = aws_array_eq_c_str((const void *)array, array_len, (const char *)c_str);

    /* --- Postconditions --- */

    /* 1. Return value is a valid bool */
    assert(result == true || result == false);

    /* 2. Frame: array memory not modified */
    if (array_len > 0) {
        for (size_t i = 0; i < array_len; i++) {
            assert(array[i] == array_snapshot[i]);
        }
    }

    /* 3. Frame: c_str memory not modified */
    for (size_t i = 0; i <= str_len; i++) {
        assert(c_str[i] == str_snapshot[i]);
    }

    /* 4. Correctness: manually compute expected result and compare */
    bool expected = true;
    const uint8_t *array_bytes = (const uint8_t *)array;
    const uint8_t *str_bytes   = (const uint8_t *)c_str;

    for (size_t i = 0; i < array_len; i++) {
        uint8_t s = str_bytes[i];
        if (s == '\0') {
            expected = false;
            break;
        }
        if (array_bytes[i] != s) {
            expected = false;
            break;
        }
    }
    if (expected) {
        /* All array_len bytes matched; check terminator */
        if (str_bytes[array_len] != '\0') {
            expected = false;
        }
    }

    assert(result == expected);

    /* 5. Special case: array_len == 0 => result iff c_str[0] == '\0' */
    if (array_len == 0) {
        assert(result == (c_str[0] == '\0'));
    }

    /* 6. If result is true, then c_str[array_len] must be '\0' */
    if (result) {
        assert(c_str[array_len] == '\0');
    }

    /* 7. If result is true and array_len > 0, all bytes must match */
    if (result && array_len > 0) {
        for (size_t i = 0; i < array_len; i++) {
            assert(array_bytes[i] == str_bytes[i]);
            assert(str_bytes[i] != '\0');
        }
    }
}
