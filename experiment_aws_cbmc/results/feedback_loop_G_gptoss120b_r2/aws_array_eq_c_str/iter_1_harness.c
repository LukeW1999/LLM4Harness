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
    /* 1. Non‑deterministic inputs */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(AWS_MEM_IS_READABLE(array, array_len));
        for (size_t i = 0; i < array_len; ++i) {
            array[i] = nondet_uint8_t();
        }
    } else {
        /* Precondition permits NULL when length is zero */
        array = NULL;
    }

    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[c_str_len] = '\0';

    /* 2. Save old state for immutability checks */
    struct store_byte_from_buffer array_store;
    if (array_len > 0) {
        save_byte_from_array(array, array_len, &array_store);
    }
    struct store_byte_from_buffer c_str_store;
    save_byte_from_array((const uint8_t *)c_str, c_str_len + 1, &c_str_store);

    /* 3. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Post‑condition on return value */
    if (result) {
        /* Success: lengths equal and all bytes match, and c_str has a terminating NUL at that index */
        assert(c_str_len == array_len);
        assert_bytes_match(array, (const uint8_t *)c_str, array_len);
        assert(c_str[array_len] == '\0');
    } else {
        /* Failure: either length mismatch, early NUL, or byte mismatch */
        bool lengths_equal = (c_str_len == array_len);
        bool all_bytes_match = true;
        if (lengths_equal) {
            for (size_t i = 0; i < array_len; ++i) {
                if (array[i] != (uint8_t)c_str[i]) {
                    all_bytes_match = false;
                    break;
                }
            }
        }
        /* The function returns false iff NOT (lengths equal && all bytes match && terminating NUL) */
        assert(!(lengths_equal && all_bytes_match && c_str[array_len] == '\0'));
    }

    /* 5. Memory must remain unchanged */
    if (array_len > 0) {
        assert_byte_from_buffer_matches(array, &array_store);
    }
    assert_byte_from_buffer_matches((const uint8_t *)c_str, &c_str_store);
}
