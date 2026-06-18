#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_eq_c_str_harness() {
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    void *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
    } else {
        if (nondet_bool()) {
            array = malloc(1);
        }
    }

    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
    
    char *c_str = malloc(str_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < str_len; ++i) {
        c_str[i] = nondet_uint8_t();
        __CPROVER_assume(c_str[i] != '\0');
    }
    c_str[str_len] = '\0';

    bool result = aws_array_eq_c_str(array, array_len, c_str);

    if (result) {
        assert(array_len == str_len);
        const uint8_t *array_bytes = (const uint8_t *)array;
        for (size_t i = 0; i < array_len; ++i) {
            assert(array_bytes[i] == (uint8_t)c_str[i]);
        }
    }
}
