#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 8

bool aws_array_eq_c_str(const void *const array, const size_t array_len, const char *const c_str);

void aws_array_eq_c_str_harness(void) {
    /* Allocate and set up inputs */
    size_t array_len;
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
    }

    /* c_str must be non-null and null-terminated within a bounded size */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    c_str[c_str_len] = '\0';

    /* Ground-truth preconditions */
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(array || (array_len == 0));
    __CPROVER_assume(c_str);

    /* Save copies of inputs for frame condition checks */
    uint8_t array_copy[MAX_BUFFER_SIZE];
    if (array != NULL && array_len > 0) {
        __CPROVER_array_copy(array_copy, array);
    }
    char c_str_copy[MAX_BUFFER_SIZE + 1];
    for (size_t i = 0; i <= c_str_len; i++) {
        c_str_copy[i] = c_str[i];
    }

    /* Call the function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* Postcondition 1: Return value correctness
     * If result is true, then array_len must equal strlen(c_str),
     * and all bytes must match. */
    if (result) {
        /* The c_str must end exactly at position array_len */
        assert(c_str[array_len] == '\0');
        /* All bytes in array must match c_str */
        for (size_t i = 0; i < array_len; i++) {
            assert(((const uint8_t *)array)[i] == (uint8_t)c_str[i]);
            /* No null terminator in the middle of c_str */
            assert(c_str[i] != '\0');
        }
    }

    /* Postcondition 2: If result is false, either lengths differ or bytes differ */
    if (!result) {
        /* Either c_str ends before array_len, or there's a mismatch, or c_str is longer */
        bool lengths_match = (c_str[array_len] == '\0');
        bool bytes_match = true;
        for (size_t i = 0; i < array_len; i++) {
            if (c_str[i] == '\0' || ((const uint8_t *)array)[i] != (uint8_t)c_str[i]) {
                bytes_match = false;
                break;
            }
        }
        assert(!lengths_match || !bytes_match);
    }

    /* Postcondition 3: Frame condition - array contents not modified */
    if (array != NULL && array_len > 0) {
        for (size_t i = 0; i < array_len; i++) {
            assert(((const uint8_t *)array)[i] == array_copy[i]);
        }
    }

    /* Postcondition 4: Frame condition - c_str contents not modified */
    for (size_t i = 0; i <= c_str_len; i++) {
        assert(c_str[i] == c_str_copy[i]);
    }

    /* Postcondition 5: Result is a valid boolean (true or false) */
    assert(result == true || result == false);
}
