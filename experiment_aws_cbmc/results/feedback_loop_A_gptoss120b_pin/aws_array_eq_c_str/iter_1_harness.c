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
    /* 1. Declare nondeterministic inputs and bound them */
    const void *array;
    size_t array_len = nondet_size_t();
    const char *c_str;

    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    /* Precondition: array may be NULL only when length is zero */
    __CPROVER_assume(array != NULL || array_len == 0);

    /* Allocate array buffer if needed */
    uint8_t *array_buf = NULL;
    if (array_len > 0) {
        array_buf = malloc(array_len);
        __CPROVER_assume(array_buf != NULL);
        /* make array contents nondeterministic */
        for (size_t i = 0; i < array_len; ++i) {
            array_buf[i] = nondet_uint8_t();
        }
    }
    array = array_buf;

    /* Allocate c_str with at least array_len+1 bytes and ensure null‑termination */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len > array_len);
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str_buf = malloc(c_str_len);
    __CPROVER_assume(c_str_buf != NULL);
    /* make c_str contents nondeterministic, then terminate */
    for (size_t i = 0; i < c_str_len - 1; ++i) {
        c_str_buf[i] = (char)nondet_uint8_t();
    }
    c_str_buf[c_str_len - 1] = '\0';
    c_str = c_str_buf;

    /* 2. Save old state for immutability checks */
    struct store_byte_from_buffer array_store;
    struct store_byte_from_buffer c_str_store;
    if (array_len > 0) {
        save_byte_from_array(array_buf, array_len, &array_store);
    }
    save_byte_from_array((uint8_t *)c_str_buf, c_str_len, &c_str_store);

    /* 3. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Compute expected result using a reference implementation */
    bool expected = true;
    for (size_t i = 0; i < array_len; ++i) {
        uint8_t s = (uint8_t)c_str_buf[i];
        if (s == '\0') {
            expected = false;
            break;
        }
        if (array_buf[i] != s) {
            expected = false;
            break;
        }
    }
    if (expected) {
        expected = (c_str_buf[array_len] == '\0');
    }

    /* 5. Assert postconditions */
    assert(result == expected);

    /* Unchanged fields / memory */
    if (array_len > 0) {
        assert_byte_from_buffer_matches(array_buf, &array_store);
    }
    assert_byte_from_buffer_matches((uint8_t *)c_str_buf, &c_str_store);
    assert(array_len == array_len); /* trivially true, keeps style consistent */
    assert(c_str_len > array_len); /* unchanged bound condition */

    /* 6. Clean up */
    free(array_buf);
    free(c_str_buf);
}
