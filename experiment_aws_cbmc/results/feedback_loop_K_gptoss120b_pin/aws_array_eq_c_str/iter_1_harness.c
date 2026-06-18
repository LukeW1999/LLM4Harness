/*=== Contract ===
Preconditions:
  - array_len is any size_t value.
  - If array_len == 0 then array may be NULL; otherwise array must be non‑NULL and point to at least array_len bytes.
  - c_str must be non‑NULL and point to a null‑terminated string (i.e., there exists some index k ≥ array_len such that c_str[k] == '\0').

Postconditions (validity):
  - The function returns true iff the first array_len characters of c_str are defined and equal to the bytes in array, and c_str[array_len] == '\0'.

Postconditions (frame):
  - The memory region pointed to by array (if non‑NULL) is not modified.
  - The memory region pointed to by c_str is not modified.
===*/

#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* prototype of the function under test */
bool aws_array_eq_c_str(const void *const array, const size_t array_len, const char *const c_str);

void aws_array_eq_c_str_harness(void) {
    /* nondet inputs */
    size_t array_len = nondet_size_t();
    const uint8_t *array = NULL;
    char *c_str = NULL;

    /* allocate array if needed */
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
        /* initialize array with nondet values */
        for (size_t i = 0; i < array_len; ++i) {
            array[i] = nondet_uint8_t();
        }
    } else {
        /* when length is zero, array may be NULL */
        __CPROVER_assume(array == NULL);
    }

    /* allocate a c string that is at least array_len+1 long and null‑terminated */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len >= array_len);
    /* ensure there is room for the terminating null */
    c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str[i] = nondet_char();
    }
    /* place a null terminator at some position >= array_len */
    size_t null_pos = nondet_size_t();
    __CPROVER_assume(null_pos >= array_len && null_pos <= c_str_len);
    c_str[null_pos] = '\0';
    /* fill any bytes after null_pos with nondet values (they are irrelevant) */
    for (size_t i = null_pos + 1; i <= c_str_len; ++i) {
        c_str[i] = nondet_char();
    }

    /* make copies of inputs for frame checking */
    uint8_t *array_copy = NULL;
    if (array_len > 0) {
        array_copy = malloc(array_len);
        __CPROVER_assume(array_copy != NULL);
        memcpy(array_copy, array, array_len);
    }
    char *c_str_copy = malloc(c_str_len + 1);
    __CPROVER_assume(c_str_copy != NULL);
    memcpy(c_str_copy, c_str, c_str_len + 1);

    /* call the function */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* compute expected result */
    bool expected = true;
    for (size_t i = 0; i < array_len; ++i) {
        if (c_str[i] == '\0' || (array != NULL && array[i] != (uint8_t)c_str[i])) {
            expected = false;
            break;
        }
    }
    if (expected) {
        expected = (c_str[array_len] == '\0');
    }

    /* assert postcondition */
    assert(result == expected);

    /* frame checks */
    if (array_len > 0) {
        assert(memcmp(array, array_copy, array_len) == 0);
    }
    assert(memcmp(c_str, c_str_copy, c_str_len + 1) == 0);

    /* clean up */
    free((void *)array);
    free(c_str);
    free(array_copy);
    free(c_str_copy);
    return 0;
}
