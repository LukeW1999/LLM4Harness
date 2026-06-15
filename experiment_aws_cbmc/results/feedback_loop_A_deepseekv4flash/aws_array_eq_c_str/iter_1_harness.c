#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_eq_c_str_harness() {
    /* Non-deterministic inputs */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    /* Allocate array buffer */
    uint8_t *array = NULL;
    if (array_len > 0) {
        array = (uint8_t *)malloc(array_len);
        __CPROVER_assume(array != NULL);
        /* Fill with non-deterministic bytes */
        for (size_t i = 0; i < array_len; i++) {
            array[i] = nondet_uint8_t();
        }
    } else {
        /* array may be NULL */
        array = NULL;
    }

    /* Allocate c_str buffer: must be at least array_len+1 bytes */
    size_t c_str_len = array_len + 1; /* ensure enough space */
    char *c_str = (char *)malloc(c_str_len);
    __CPROVER_assume(c_str != NULL);
    /* Fill with non-deterministic bytes, but ensure last byte is null */
    for (size_t i = 0; i < array_len; i++) {
        c_str[i] = nondet_uint8_t();
    }
    c_str[array_len] = '\0'; /* null terminator */

    /* Save old state of arrays */
    uint8_t *old_array = NULL;
    if (array_len > 0) {
        old_array = (uint8_t *)malloc(array_len);
        __CPROVER_assume(old_array != NULL);
        memcpy(old_array, array, array_len);
    }
    char *old_c_str = (char *)malloc(c_str_len);
    __CPROVER_assume(old_c_str != NULL);
    memcpy(old_c_str, c_str, c_str_len);

    /* Call function */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* Postconditions */
    /* 1. Memory unchanged */
    if (array_len > 0) {
        assert_bytes_match(array, old_array, array_len);
    }
    assert_bytes_match((uint8_t *)c_str, (uint8_t *)old_c_str, c_str_len);

    /* 2. Return value consistency */
    if (result) {
        /* All bytes match and c_str[array_len] == '\0' */
        for (size_t i = 0; i < array_len; i++) {
            assert(array[i] == (uint8_t)c_str[i]);
        }
        assert(c_str[array_len] == '\0');
    } else {
        /* Either a mismatch or c_str[array_len] != '\0' */
        /* No further assertion needed */
    }

    /* 3. No side effects on pointers (they are not modified) */
    /* The function does not modify any pointers, so no need to assert pointer equality. */

    /* 4. Validity invariants: none for raw pointers. */
}
