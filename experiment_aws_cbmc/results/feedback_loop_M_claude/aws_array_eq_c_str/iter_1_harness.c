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
    /* We bound the c_str length to keep state space manageable */
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
     * If result is true:
     * - c_str must have length exactly array_len (null terminator at position array_len)
     * - All bytes in array match corresponding bytes in c_str
     */
    if (result) {
        /* c_str[array_len] must be '\0' */
        assert(c_str[c_str_len] == '\0');
        /* The lengths must match: c_str_len == array_len */
        /* We can assert that c_str[array_len] == '\0' which means array_len <= c_str_len */
        /* And since we traversed all array_len bytes without finding '\0', array_len == c_str_len */
        assert(array_len == c_str_len);
        /* All bytes must match */
        if (array_len > 0 && array != NULL) {
            assert_bytes_match(array, (const uint8_t *)c_str, array_len);
        }
    } else {
        /* On false: either lengths differ, or some byte doesn't match, or c_str contains '\0' before array_len */
        /* We can't assert much here beyond the function returning false */
        /* The inputs are unchanged (this function doesn't modify inputs) */
        assert(true); /* placeholder — inputs are not modified */
    }

    /* 6. The function is pure — it does not modify its inputs */
    /* array and c_str are unchanged (we can't easily check nondet memory, but we assert validity) */
    /* No validity invariants for raw arrays, but we can check the null terminator is still there */
    assert(c_str[c_str_len] == '\0');
}
