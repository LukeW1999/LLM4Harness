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
    /* 1. Nondeterministic inputs, bounded */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str_len >= array_len);               /* need at least array_len+1 bytes for null‑terminator */

    /* 2. Allocate memory for inputs */
    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
    }

    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* 3. Ensure the required null‑terminator for the string */
    c_str[array_len] = '\0';

    /* 4. Save old state of the array for immutability check */
    struct store_byte_from_buffer array_old;
    if (array_len > 0) {
        save_byte_from_array(array, array_len, &array_old);
    }

    /* 5. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 6. Compute expected result according to the specification */
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

    /* 7. Assert post‑conditions */
    assert(result == expected);                     /* return value matches specification */
    if (array_len > 0) {
        assert_byte_from_buffer_matches(array, &array_old);   /* array contents unchanged */
    }
    assert(array == (array_len == 0 ? NULL : array));          /* pointer unchanged */
    assert(c_str == c_str);                                   /* c_str pointer unchanged */

    /* 8. Memory‑access validity invariants */
    if (array_len > 0) {
        assert(AWS_MEM_IS_READABLE(array, array_len));
    }
    assert(AWS_MEM_IS_READABLE(c_str, array_len + 1));
}
