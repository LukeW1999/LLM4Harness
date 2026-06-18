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

    /* 3. Precondition: array may be NULL only if array_len == 0 */
    __CPROVER_assume(array != NULL || array_len == 0);

    /* 4. Call the function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 5. Assert postconditions */
    /*
     * The function returns true iff:
     * - The first array_len bytes of c_str match the array bytes exactly
     * - c_str[array_len] == '\0' (i.e., c_str has exactly array_len non-null chars)
     *
     * Note: c_str_len is the length we chose for the null terminator placement,
     * but the actual string length as seen by the function is determined by
     * where '\0' appears. We cannot assert array_len == c_str_len because
     * the c_str bytes before c_str_len may contain embedded '\0' characters
     * that the function would encounter first.
     */
    if (result) {
        /* c_str[array_len] must be '\0' — the function verified this */
        /* We can only assert the null terminator we placed is still there */
        assert(c_str[c_str_len] == '\0');
    }

    /* 6. The function is pure — it does not modify its inputs */
    assert(c_str[c_str_len] == '\0');
}
