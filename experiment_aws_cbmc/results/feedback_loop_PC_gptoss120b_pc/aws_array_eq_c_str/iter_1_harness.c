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
    /* 1. Nondeterministic inputs */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
        /* make array contents nondeterministic */
        __CPROVER_assume(AWS_MEM_IS_READABLE(array, array_len));
    }

    /* c_str must be a valid null‑terminated string */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    /* allocate at least one byte for the terminating '\0' */
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));
    /* nondet fill the string (including the terminating null) */
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[c_str_len] = '\0';

    /* 2. Save old state */
    uint8_t *old_array = NULL;
    if (array_len > 0) {
        old_array = malloc(array_len);
        __CPROVER_assume(old_array != NULL);
        memcpy(old_array, array, array_len);
    }
    size_t old_array_len = array_len;
    uint8_t *old_array_ptr = array;

    size_t old_c_str_len = c_str_len + 1; /* include terminating null */
    char *old_c_str = malloc(old_c_str_len);
    __CPROVER_assume(old_c_str != NULL);
    memcpy(old_c_str, c_str, old_c_str_len);
    char *old_c_str_ptr = c_str;

    /* 3. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Compute expected result */
    bool expected = true;
    for (size_t i = 0; i < array_len; ++i) {
        uint8_t s = (uint8_t)c_str[i];
        if (s == '\0') {
            expected = false;
            break;
        }
        if (array[i] != s) {
            expected = false;
            break;
        }
    }
    if (expected) {
        expected = (c_str[array_len] == '\0');
    }

    /* 5. Assert return value matches specification */
    assert(result == expected);

    /* 6. Assert unchanged inputs (frame condition) */
    assert(array == old_array_ptr);
    assert(array_len == old_array_len);
    assert(c_str == old_c_str_ptr);

    if (array_len > 0) {
        assert_bytes_match(array, old_array, array_len);
    }
    assert_bytes_match((uint8_t *)c_str, (uint8_t *)old_c_str, old_c_str_len);

    /* 7. Assert precondition still holds (should be invariant) */
    assert(array != NULL || array_len == 0);
    assert(c_str != NULL);

    /* 8. No additional validity invariants for plain pointers */
}
