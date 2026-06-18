#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>

void aws_array_eq_c_str_harness() {
    /* nondet inputs */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    /* array */
    void *array;
    if (nondet_bool()) {
        /* array may be NULL only when array_len is zero */
        __CPROVER_assume(array_len == 0);
        array = NULL;
    } else {
        if (array_len > 0) {
            array = malloc(array_len);
            __CPROVER_assume(array != NULL);
            __CPROVER_assume(AWS_MEM_IS_READABLE(array, array_len));
        } else {
            array = malloc(1); /* we allocate a dummy byte for consistency */
            __CPROVER_assume(array != NULL);
        }
    }

    /* c_str: must be readable for at least array_len+1 bytes */
    size_t c_str_buf_size = array_len + 1;
    char *c_str = malloc(c_str_buf_size);
    __CPROVER_assume(c_str != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_buf_size));

    /* save original state for immutability check (first byte of each buffer) */
    struct store_byte_from_buffer array_storage;
    struct store_byte_from_buffer c_str_storage;
    if (array_len > 0) {
        save_byte_from_array((const uint8_t *)array, array_len, &array_storage);
    }
    save_byte_from_array((const uint8_t *)c_str, c_str_buf_size, &c_str_storage);

    /* reference model */
    bool ref_result;
    {
        const uint8_t *array_bytes = (const uint8_t *)array;
        const uint8_t *str_bytes = (const uint8_t *)c_str;

        bool match = true;
        size_t i;
        for (i = 0; i < array_len; ++i) {
            uint8_t s = str_bytes[i];
            if (s == '\0') {
                match = false;
                break;
            }
            if (array_bytes[i] != s) {
                match = false;
                break;
            }
        }
        if (match) {
            ref_result = (str_bytes[array_len] == '\0');
        } else {
            ref_result = false;
        }
    }

    /* call the function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* postconditions */

    /* 1. Return value matches the reference model */
    assert(result == ref_result);

    /* 2. Memory pointed to by array and c_str must not be modified */
    if (array_len > 0) {
        assert_byte_from_buffer_matches((const uint8_t *)array, &array_storage);
    }
    assert_byte_from_buffer_matches((const uint8_t *)c_str, &c_str_storage);

    /* 3. No pointers changed (const guarantees but explicit check) */
    /* (already satisfied by const qualifiers) */

    /* 4. No additional safety requirements beyond the precondition */
    /* (CBMC automatically checks memory safety under the given `assume`s) */
}
