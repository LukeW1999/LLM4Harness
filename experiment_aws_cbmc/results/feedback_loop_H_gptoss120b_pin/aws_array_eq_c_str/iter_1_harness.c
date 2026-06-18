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
    /* 1. Nondeterministic inputs */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    const void *array;
    uint8_t *array_buf = NULL;
    if (array_len > 0) {
        array_buf = malloc(array_len);
        __CPROVER_assume(array_buf != NULL);
        /* nondeterministic contents */
        for (size_t i = 0; i < array_len; ++i) {
            array_buf[i] = nondet_uint8_t();
        }
        array = array_buf;
    } else {
        array = NULL;
    }

    /* Precondition from AWS_PRECONDITION */
    __CPROVER_assume(array != NULL || array_len == 0);

    /* c_str must be a null‑terminated string with at least array_len+1 readable bytes */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len >= array_len + 1);
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    char *c_str = malloc(c_str_len);
    __CPROVER_assume(c_str != NULL);
    /* ensure there is a terminating NUL somewhere within the allocated range */
    for (size_t i = 0; i < c_str_len - 1; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[c_str_len - 1] = '\0'; /* guarantee termination */

    /* 2. Save old state */
    uint8_t *old_array = NULL;
    if (array_len > 0) {
        old_array = malloc(array_len);
        __CPROVER_assume(old_array != NULL);
        memcpy(old_array, array_buf, array_len);
    }
    char *old_c_str = malloc(c_str_len);
    __CPROVER_assume(old_c_str != NULL);
    memcpy(old_c_str, c_str, c_str_len);
    const void *old_array_ptr = array;
    const char *old_c_str_ptr = c_str;
    size_t old_array_len = array_len;
    size_t old_c_str_len = c_str_len;

    /* 3. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Post‑condition: result matches the specification */
    bool expected = true;
    for (size_t i = 0; i < array_len; ++i) {
        if ((uint8_t)c_str[i] == '\0' || ((uint8_t *)array)[i] != (uint8_t)c_str[i]) {
            expected = false;
            break;
        }
    }
    if (expected) {
        expected = ((uint8_t)c_str[array_len] == '\0');
    }
    assert(result == expected);

    /* 5. Unchanged inputs */
    assert(array == old_array_ptr);
    assert(array_len == old_array_len);
    assert(c_str == old_c_str_ptr);
    assert(c_str_len == old_c_str_len);
    if (array_len > 0) {
        assert_bytes_match(array_buf, old_array, array_len);
    }
    assert_bytes_match((uint8_t *)c_str, (uint8_t *)old_c_str, c_str_len);

    /* 6. No memory leaks / validity (nothing to validate beyond the above) */
    /* free allocated memory */
    free(array_buf);
    free(old_array);
    free(c_str);
    free(old_c_str);
}
