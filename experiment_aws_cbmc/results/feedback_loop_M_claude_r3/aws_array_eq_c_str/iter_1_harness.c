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

    /* 2. Set up the c_str input — must be null-terminated */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    /* Allocate c_str_len + 1 bytes to hold the string plus null terminator */
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Ensure null terminator is present */
    c_str[c_str_len] = '\0';

    /* 3. Call the function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Assert postconditions */

    /* If result is true, then:
     * - array_len must equal strlen(c_str) (i.e., c_str_len)
     * - all bytes in array must match the corresponding bytes in c_str
     * - c_str must not contain a null terminator before position array_len
     */
    if (result) {
        /* The lengths must match */
        assert(array_len == c_str_len);

        /* All bytes must match */
        for (size_t i = 0; i < array_len; i++) {
            assert(array[i] == (uint8_t)c_str[i]);
            /* No null terminator in the middle */
            assert(c_str[i] != '\0');
        }

        /* The c_str must be null-terminated exactly at array_len */
        assert(c_str[array_len] == '\0');
    } else {
        /* On false result, either:
         * - lengths differ (c_str has a '\0' before array_len, or c_str[array_len] != '\0')
         * - or some byte differs
         * We can't assert much here beyond the function returning false
         * but we can verify the inputs are unchanged (no side effects)
         */
        /* The function is pure — no side effects to check */
        /* Just verify the result is indeed false */
        assert(!result);
    }

    /* 5. Verify the function handles the NULL array case correctly:
     * If array_len == 0, array may be NULL, and the function should still work */
    if (array_len == 0) {
        /* result depends only on whether c_str[0] == '\0' */
        if (result) {
            assert(c_str[0] == '\0');
        } else {
            assert(c_str[0] != '\0');
        }
    }
}
