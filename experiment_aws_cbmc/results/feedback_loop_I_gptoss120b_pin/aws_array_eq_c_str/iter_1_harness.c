#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_array_eq_c_str_harness(void) {
    /* 1. nondeterministic inputs */
    const void *array;
    size_t array_len = nondet_size_t();
    const char *c_str;

    /* bound lengths */
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    /* allocate array if needed */
    if (array_len > 0) {
        uint8_t *buf = malloc(array_len);
        __CPROVER_assume(buf != NULL);
        array = buf;
    } else {
        array = NULL;
    }

    /* allocate a null‑terminated C string */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len > 0);
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_buf = malloc(c_str_len);
    __CPROVER_assume(c_buf != NULL);
    /* place a null terminator at a nondeterministic position */
    size_t null_pos = nondet_size_t();
    __CPROVER_assume(null_pos < c_str_len);
    c_buf[null_pos] = '\0';
    c_str = c_buf;

    /* 2. precondition from AWS_PRECONDITION */
    __CPROVER_assume(array != NULL || array_len == 0);

    /* 3. save old state of the array (if any) */
    struct store_byte_from_buffer old_array_bytes = {0};
    if (array_len > 0) {
        save_byte_from_array((const uint8_t *)array, array_len, &old_array_bytes);
    }

    /* 4. call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 5. compute expected result according to the specification */
    bool expected = true;
    const uint8_t *array_bytes = (const uint8_t *)array;
    const uint8_t *str_bytes   = (const uint8_t *)c_str;
    for (size_t i = 0; i < array_len; ++i) {
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
        expected = (str_bytes[array_len] == '\0');
    }

    /* 6. post‑condition: return value matches specification */
    assert(result == expected);

    /* 7. post‑condition: array contents unchanged */
    if (array_len > 0) {
        assert_byte_from_buffer_matches((const uint8_t *)array, &old_array_bytes);
    }

    /* 8. clean up */
    free((void *)array);
    free((void *)c_str);
}
